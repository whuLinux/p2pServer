#include "udpserverutil.h"

UDPServerUtil::UDPServerUtil()
{
    this->msgUtil = new MsgUtil();
    this->outOfDate = true;
}

UDPServerUtil::UDPServerUtil(quint16 port) : port(port)
{
    this->msgUtil = new MsgUtil();
    this->outOfDate = true;
}

UDPServerUtil::~UDPServerUtil()
{
    delete this->server;
    delete this->msgUtil;

    this->server = nullptr;
    this->msgUtil = nullptr;
}

void UDPServerUtil::initTimer() {
    this->timer = new QTimer();
    connect(this->timer, SIGNAL(timeout()), this, SLOT(updateClient()));
    timer->start(1000 * 60 * 60 * 24 * 7);
}

bool UDPServerUtil::stablishServer()
{
    createSocket();

    if (!listen()) {
        qDebug() << "UDPServerUtil::stablishServer " << "服务器端无法监听端口" << endl;
        qDebug() << "UDPServerUtil::stablishServer " << this->server->errorString() << endl;

        return false;
    }

    connect(this->server, SIGNAL(readyRead()), this, SLOT(recfromClient()));
    initTimer();

    return true;
}

bool UDPServerUtil::createSocket()
{
    this->server = new QUdpSocket();
    return true;
}

bool UDPServerUtil::listen()
{
    if (!this->server->bind(QHostAddress::AnyIPv4, this->port)) {
        qDebug() << "UDPServerUtil::listen " << "服务器端无法监听端口" << endl;
        qDebug() << "UDPServerUtil::listen " << this->server->errorString() << endl;

        this->server->close();
        return false;
    }

    qDebug() << "UDPServerUtil::listen " << "服务器端正在监听端口 " << this->port << endl;
    return true;
}

void UDPServerUtil::rename(QHostAddress ip, quint16 udpPort)
{
    CtrlMsg msg = this->msgUtil->createRenameMsg();
    this->server->writeDatagram(msg.toMsg(), ip, udpPort);
}

void UDPServerUtil::loginSuccess(QHostAddress ip, quint16 udpPort)
{
    CtrlMsg msg = this->msgUtil->createLoginSuccessMsg();
    this->server->writeDatagram(msg.toMsg(), ip, udpPort);
}

void UDPServerUtil::loginFailure(QHostAddress ip, quint16 udpPort)
{
    CtrlMsg msg = this->msgUtil->createLoginFailureMsg();
    this->server->writeDatagram(msg.toMsg(), ip, udpPort);
}

void UDPServerUtil::logoutSuccess(QHostAddress ip, quint16 udpPort)
{
    CtrlMsg msg = this->msgUtil->createLogoutSuccessMsg();
    this->server->writeDatagram(msg.toMsg(), ip, udpPort);
}

void UDPServerUtil::logoutFailure(QHostAddress ip, quint16 udpPort)
{
    CtrlMsg msg = this->msgUtil->createLogoutFailureMsg();
    this->server->writeDatagram(msg.toMsg(), ip, udpPort);
}

void UDPServerUtil::sendAllPartners(QHostAddress ip, quint16 udpPort, QString hostName)
{
    QVector<ClientNode> clients;
    QMap<QString, ClientNode>::iterator it;

    for (it = this->clientNodes.begin(); it != this->clientNodes.end(); it++) {
        if (it.key() == hostName) {
            continue;
        }

        ClientNode client;
        client.name = it.value().name;
        client.ip = it.value().ip;
        client.port = it.value().port;
        client.filePort = it.value().filePort;
        clients.append(client);
    }

    CtrlMsg msg = this->msgUtil->createReturnAllPartners(clients);
    this->server->writeDatagram(msg.toMsg(), ip, udpPort);
}

void UDPServerUtil::obtainSuccess(QHostAddress ip, quint16 udpPort)
{
    CtrlMsg msg = this->msgUtil->createObtainSuccessMsg();
    this->server->writeDatagram(msg.toMsg(), ip, udpPort);
}

void UDPServerUtil::obtainFailure(QHostAddress ip, quint16 udpPort)
{
    CtrlMsg msg = this->msgUtil->createObtainFailureMsg();
    this->server->writeDatagram(msg.toMsg(), ip, udpPort);
}

void UDPServerUtil::p2pNeedHole(QString name)
{
    ClientNode client;
    client.name = name;
    client.ip = this->clientNodes[name].ip;
    client.port = this->clientNodes[name].port;
    client.filePort = this->clientNodes[name].filePort;

    CtrlMsg msg = this->msgUtil->createP2PHolePackage(client);
    this->server->writeDatagram(msg.toMsg(), QHostAddress(this->clientNodes[name].ip), this->clientNodes[name].udpPort);
}

bool UDPServerUtil::recfromClient()
{
    this->outOfDate = false;

    while (this->server->hasPendingDatagrams()) {
        qDebug() << "接收客户端信息" << endl;

        QByteArray datagram;
        QHostAddress peerHost;
        quint16 peerPort;

        datagram.resize(int(this->server->pendingDatagramSize()));
        this->server->readDatagram(datagram.data(), datagram.size(), &peerHost, &peerPort);

        QJsonObject jsonMsg;
        jsonMsg = QJsonDocument::fromJson(datagram).object();

        if (jsonMsg.value(MSGTYPE).isUndefined()) {
            qDebug() << "UDPServerUtil::recfromClient " << "无法解析客户端发来的消息类型" << endl;
            return false;
        }

        qDebug() << jsonMsg.value(MSGTYPE).toInt() << endl;

        if (jsonMsg.value(MSGTYPE).toInt() == qint8(UDPCtrlMsgType::LOGIN)) {
            return login(peerHost, peerPort, jsonMsg);
        } else if (jsonMsg.value(MSGTYPE).toInt() == qint8(UDPCtrlMsgType::LOGOUT)) {
            return logout(peerHost, peerPort, jsonMsg);
        } else if (jsonMsg.value(MSGTYPE).toInt() == qint8(UDPCtrlMsgType::OBTAINALLPARTNERS)) {
            return wantAllPartners(peerHost, peerPort, jsonMsg);
        } else if (jsonMsg.value(MSGTYPE).toInt() == qint8(UDPCtrlMsgType::P2PTRANS)) {
            return p2pTrans(jsonMsg);
        }
    }

    return false;
}

bool UDPServerUtil::login(QHostAddress ip, quint16 udpPort, QJsonObject & jsonMsg)
{
    if (jsonMsg.value(HOSTNAME).isUndefined() || jsonMsg.value(PWD).isUndefined() || jsonMsg.value(PORT).isUndefined() || jsonMsg.value(FILEPORT).isUndefined()) {
        qDebug() << "UDPServerUtil::login " << "客户端发来的消息不完整" << endl;
        loginFailure(ip, udpPort);

        return false;
    }

    QString hostName = jsonMsg.value(HOSTNAME).toString();
    QString pwd = jsonMsg.value(PWD).toString();
    quint16 port = quint16(jsonMsg.value(PORT).toInt());
    quint16 filePort = quint16(jsonMsg.value(FILEPORT).toInt());

    if (this->clientNodes.contains(hostName) && this->clientNodes[hostName].pwd == pwd) {
        qDebug() << "UDPServerUtil::login " << "主机名已经注册，请修改主机名" << endl;
        rename(ip, udpPort);

        return false;
    }

    this->clientNodes[hostName].name = hostName;
    this->clientNodes[hostName].ip = ip.toString();
    this->clientNodes[hostName].pwd = pwd;
    this->clientNodes[hostName].port = port;
    this->clientNodes[hostName].udpPort = udpPort;
    this->clientNodes[hostName].filePort = filePort;

    qDebug() << this->clientNodes[hostName].name << this->clientNodes[hostName].ip << this->clientNodes[hostName].pwd << this->clientNodes[hostName].port << this->clientNodes[hostName].filePort << this->clientNodes[hostName].udpPort;
    loginSuccess(ip, udpPort);

    return true;
}

bool UDPServerUtil::logout(QHostAddress ip, quint16 udpPort, QJsonObject & jsonMsg)
{
    if (jsonMsg.value(HOSTNAME).isUndefined() || jsonMsg.value(PWD).isUndefined()) {
        qDebug() << "UDPServerUtil::logout " << "客户端发来的消息不完整" << endl;
        logoutFailure(ip, udpPort);

        return false;
    }

    QString hostName = jsonMsg.value(HOSTNAME).toString();
    QString pwd = jsonMsg.value(PWD).toString();

    if (this->clientNodes.contains(hostName) && this->clientNodes[hostName].pwd != pwd) {
        qDebug() << "UDPServerUtil::logout " << "客户端密码错误，认证失败" << endl;
        logoutFailure(ip, udpPort);
        return false;
    }

    this->clientNodes.remove(hostName);
    logoutSuccess(ip, udpPort);

    return true;
}

bool UDPServerUtil::p2pTrans(QJsonObject & jsonMsg)
{
    if (jsonMsg.value(HOSTNAME).isUndefined() || jsonMsg.value(PWD).isUndefined() || jsonMsg.value(PARTNERNAME).isUndefined()) {
        qDebug() << "UDPServerUtil::p2pTrans " << "客户端发来的消息不完整" << endl;
        return false;
    }

    QString hostName = jsonMsg.value(HOSTNAME).toString();
    QString pwd = jsonMsg.value(PWD).toString();

    if (this->clientNodes.contains(hostName) && this->clientNodes[hostName].pwd != pwd) {
        qDebug() << "UDPServerUtil::p2pTrans " << "客户端密码错误，认证失败" << endl;
        return false;
    }

    QString partnerName = jsonMsg.value(PARTNERNAME).toString();
    if (this->clientNodes.contains(partnerName)) {
        qDebug() << "UDPServerUtil::p2pTrans " << "目标客户端尚未注册" << endl;
        return false;
    }

    p2pNeedHole(partnerName);
    return true;
}

bool UDPServerUtil::wantAllPartners(QHostAddress ip, quint16 udpPort, QJsonObject & jsonMsg)
{
    if (jsonMsg.value(HOSTNAME).isUndefined() || jsonMsg.value(PWD).isUndefined()) {
        qDebug() << "UDPServerUtil::wantAllPartners " << "客户端发来的消息不完整" << endl;
        logoutFailure(ip, udpPort);

        return false;
    }

    QString hostName = jsonMsg.value(HOSTNAME).toString();
    QString pwd = jsonMsg.value(PWD).toString();

    if (this->clientNodes.contains(hostName) && this->clientNodes[hostName].pwd != pwd) {
        qDebug() << "UDPServerUtil::wantAllPartners " << "客户端密码错误，认证失败" << endl;
        logoutFailure(ip, udpPort);
        return false;
    }

    sendAllPartners(ip, udpPort, hostName);
    return true;
}

