#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include "udpserverutil.h"

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
