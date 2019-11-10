#include "ctrlmsg.h"

CtrlMsg::CtrlMsg()
{

}

CtrlMsg::CtrlMsg(UDPCtrlMsgType msgType) : msgType(msgType)
{

}

CtrlMsg::~CtrlMsg()
{
    this->clients.clear();
}

QByteArray CtrlMsg::toMsg()
{
    QJsonObject jsonMsg {
        {MSGTYPE, qint8(this->msgType)}
    };

    if (this->msgType == UDPCtrlMsgType::RETURNALLPARTNERS) {
        QJsonArray jsonClients;

        int clientNum = this->clients.size();
        for (int i = 0; i < clientNum; i++) {
            QJsonObject jsonClient {
                {PARTNERNAME, this->clients.at(i).name},
                {IP, this->clients.at(i).ip},
                {PORT, this->clients.at(i).port}
            };

            jsonClients.append(jsonClient);
        }

        jsonMsg.insert(PARTNERVECTOR, jsonClients);

    } else if (this->msgType == UDPCtrlMsgType::P2PNEEDHOLE) {
        if (!this->clients.empty()) {
            QJsonObject jsonClient {
                {PARTNERNAME, this->clients.at(0).name},
                {IP, this->clients.at(0).ip},
                {PORT, this->clients.at(0).port}
            };

            jsonMsg.insert(FRIEND, jsonClient);

        }
    }

    return QJsonDocument(jsonMsg).toJson();
}
