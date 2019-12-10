#ifndef CTRLMSG_H
#define CTRLMSG_H

#include <QVector>

#include <QByteArray>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "uniformlabel.h"

/**
 * @brief The CtrlmMsg class
 * @author 余宗宪
 * 服务器和客户端UDP通信的控制消息
 */
class CtrlMsg
{
private:
    UDPCtrlMsgType msgType;

    /**
     * 客户端信息
     */
    QVector<ClientNode> clients;

public:
    CtrlMsg();
    CtrlMsg(UDPCtrlMsgType msgType);

    ~CtrlMsg();

    inline void setClients(QVector<ClientNode> & clients);
    inline void addClient(ClientNode & client);

    QByteArray toMsg();

    inline UDPCtrlMsgType getMsgType();
};

void CtrlMsg::setClients(QVector<ClientNode> & clients)
{
    this->clients = clients;
}

void CtrlMsg::addClient(ClientNode & client)
{
    this->clients.push_back(client);
}

UDPCtrlMsgType CtrlMsg::getMsgType()
{
    return this->msgType;
}

#endif // CTRLMSG_H

