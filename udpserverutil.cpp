#include "udpserverutil.h"

UDPServerUtil::UDPServerUtil()
{

}

UDPServerUtil::UDPServerUtil(quint16 port) : port(port)
{

}

UDPServerUtil::~UDPServerUtil()
{
    delete this->server;
    this->server = nullptr;
}

bool UDPServerUtil::stablishServer()
{
    createSocket();

    if (!listen()) {
        qDebug() << "UDPServerUtil::stablishServer " << "服务器端无法监听端口" << endl;
        qDebug() << "UDPServerUtil::stablishServer " << this->server->errorString() << endl;

        return false;
    }

    connect(this->server, SIGNAL(readyRead()), this, SLOT(recfromClient));
    return true;
}

bool UDPServerUtil::createSocket()
{
    this->server = new QUdpSocket();
    return true;
}

bool UDPServerUtil::listen()
{
    if (!this->server->bind(QHostAddress::LocalHost, this->port)) {
        qDebug() << "UDPServerUtil::listen " << "服务器端无法监听端口" << endl;
        qDebug() << "UDPServerUtil::listen " << this->server->errorString() << endl;

        this->server->close();
        return false;
    }

    return true;
}

void UDPServerUtil::rename(QHostAddress ip, quint16 udpPort)
{
    CtrlMsg msg = this->msgUtil.createRenameMsg();
    this->server->writeDatagram(msg.toMsg(), ip, udpPort);
}

void UDPServerUtil::loginSuccess(QHostAddress ip, quint16 udpPort)
{
    CtrlMsg msg = this->msgUtil.createLoginSuccessMsg();
    this->server->writeDatagram(msg.toMsg(), ip, udpPort);
}

void UDPServerUtil::loginFailure(QHostAddress ip, quint16 udpPort)
{
    CtrlMsg msg = this->msgUtil.createLoginFailureMsg();
    this->server->writeDatagram(msg.toMsg(), ip, udpPort);
}

void UDPServerUtil::logoutSuccess(QHostAddress ip, quint16 udpPort)
{
    CtrlMsg msg = this->msgUtil.createLogoutSuccessMsg();
    this->server->writeDatagram(msg.toMsg(), ip, udpPort);
}

void UDPServerUtil::logoutFailure(QHostAddress ip, quint16 udpPort)
{
    CtrlMsg msg = this->msgUtil.createLogoutFailureMsg();
    this->server->writeDatagram(msg.toMsg(), ip, udpPort);
}

void UDPServerUtil::p2pNeedHole(QString name)
{
    CtrlMsg msg = this->msgUtil.createP2PHolePackage(this->clientNodes[name]);
    this->server->writeDatagram(msg.toMsg(), QHostAddress(this->clientNodes[name].ip), this->clientNodes[name].udpPort);
}

void UDPServerUtil::sendAllPartners(QHostAddress ip, quint16 udpPort)
{
    QVector<ClientNode> clients;
    QMap<QString, ClientNode>::iterator it;

    for (it = this->clientNodes.begin(); it != this->clientNodes.end(); it++) {
        ClientNode client;
        client.name = it.value().name;
        client.ip = it.value().ip;
        client.port = it.value().port;
        clients.append(client);
    }

    CtrlMsg msg = this->msgUtil.createReturnAllPartners(clients);
    this->server->writeDatagram(msg.toMsg(), ip, udpPort);
}

bool UDPServerUtil::recfromClient()
{
    while (this->server->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(int(this->server->pendingDatagramSize()));

        this->server->readDatagram(datagram.data(), datagram.size());

        QJsonObject jsonMsg;
        jsonMsg = QJsonDocument::fromJson(datagram).object();

        if (jsonMsg.value(MSGTYPE).isUndefined()) {
            qDebug() << "UDPServerUtil::recfromClient " << "无法解析客户端发来的消息类型" << endl;
            return false;
        }

        if (jsonMsg.value(MSGTYPE).toInt() == qint8(UDPCtrlMsgType::LOGIN)) {
            return login(this->server->peerAddress(), this->server->peerPort(), jsonMsg);
        } else if (jsonMsg.value(MSGTYPE).toInt() == qint8(UDPCtrlMsgType::LOGOUT)) {
            return logout(this->server->peerAddress(), this->server->peerPort(), jsonMsg);
        } else if (jsonMsg.value(MSGTYPE).toInt() == qint8(UDPCtrlMsgType::OBTAINALLPARTNERS)) {
            return wantAllPartners(this->server->peerAddress(), this->server->peerPort());
        } else if (jsonMsg.value(MSGTYPE).toInt() == qint8(UDPCtrlMsgType::P2PTRANS)) {
            return p2pTrans(jsonMsg);
        }
    }

    return false;
}

bool UDPServerUtil::login(QHostAddress ip, quint16 udpPort, QJsonObject & jsonMsg)
{
    if (jsonMsg.value(HOSTNAME).isUndefined() || jsonMsg.value(PWD).isUndefined() || jsonMsg.value(PORT).isUndefined() || jsonMsg.value(UDPPORT).isUndefined()) {
        qDebug() << "UDPServerUtil::login " << "客户端发来的消息不完整" << endl;
        loginFailure(ip, udpPort);

        return false;
    }

    QString hostName = jsonMsg.value(HOSTNAME).toString();
    QString pwd = jsonMsg.value(PWD).toString();
    quint16 port = quint16(jsonMsg.value(PORT).toInt());

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
        qDebug() << "UDPServerUtil::login " << "客户端密码错误，认证失败" << endl;
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

bool UDPServerUtil::wantAllPartners(QHostAddress ip, quint16 udpPort)
{
    sendAllPartners(ip, udpPort);
    return true;
}

