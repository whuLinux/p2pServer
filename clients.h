#ifndef CLIENTS_H
#define CLIENTS_H

#include <QMap>

#include "uniformlabel.h"

class Clients
{
private:
    QMap<QString, ClientNode> clientNodes;

public:
    Clients();

    ~Clients();

    bool addClient(QString name, QString pwd, QString ip, quint16 port, quint16 udpPort);
    bool removeClient(QString ame, QString pwd);

    inline bool hasClient(QString name);
    inline void clearClient();
};

bool Clients::hasClient(QString name)
{
    return this->clientNodes.contains(name);
}

void Clients::clearClient()
{
    this->clientNodes.clear();
}

#endif // CLIENTS_H
