#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include "udpserverutil.h"

/**
 * @brief The Server class
 * @author 余宗宪
 * P2P通信模块的服务器
 */
class Server : QObject
{
    Q_OBJECT
private:
    UDPServerUtil * util;
    quint16 port;

public:
    Server();
    ~Server();

    bool startWorking();

};

#endif // SERVER_H
