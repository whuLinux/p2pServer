#ifndef UDPSERVERUTIL_H
#define UDPSERVERUTIL_H

#include <QObject>
#include <QTimer>

#include <QUdpSocket>
#include <QHostAddress>

#include "ctrlmsg.h"
#include "msgutil.h"
#include "uniformlabel.h"

class UDPServerUtil : public QObject
{
    Q_OBJECT
private:
    /**
     * 客户端信息
     */
    QMap<QString, ClientNode> clientNodes;

    /**
     * 服务器信息
     */
    QUdpSocket * server;
    quint16 port;

    MsgUtil * msgUtil;

    QTimer * timer;
    bool outOfDate;

public:
    UDPServerUtil();
    UDPServerUtil(quint16 port);

    ~UDPServerUtil();

    inline void setPort(quint16 port);

    void initTimer();

    bool stablishServer();
    bool createSocket();
    bool listen();

    /**
     * @brief 向客户端发送消息
     */
    void rename(QHostAddress ip, quint16 udpPort);
    void loginSuccess(QHostAddress ip, quint16 udpPort);
    void loginFailure(QHostAddress ip, quint16 udpPort);
    void logoutSuccess(QHostAddress ip, quint16 udpPort);
    void logoutFailure(QHostAddress ip, quint16 udpPort);
    void sendAllPartners(QHostAddress ip, quint16 udpPort, QString hostName);
    void obtainSuccess(QHostAddress ip, quint16 udpPort);
    void obtainFailure(QHostAddress ip, quint16 udpPort);
    void p2pNeedHole(QString hostName, QString partnerName);

    /**
     * @brief 接收客户端消息
     * p2pTrans wantAllPartners 出错不返回信息，留待客户端重传
     */
    bool login(QHostAddress ip, quint16 udpPort, QJsonObject & jsonMsg);
    bool logout(QHostAddress ip, quint16 udpPort, QJsonObject & jsonMsg);
    bool p2pTrans(QJsonObject & jsonMsg);
    bool wantAllPartners(QHostAddress ip, quint16 udpPort, QJsonObject & jsonMsg);

public slots:
    bool recfromClient();

    /**
     * @brief 长时间没有请求，强制清除过期客户端信息
     */
    inline void updateClient();

};

void UDPServerUtil::setPort(quint16 port)
{
    this->port = port;
}

void UDPServerUtil::updateClient()
{
    if (this->outOfDate) {
        this->clientNodes.clear();
    }

    this->outOfDate = true;
}

#endif // UDPSERVERUTIL_H
