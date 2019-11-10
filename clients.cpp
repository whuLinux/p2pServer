#include "clients.h"

Clients::Clients()
{

}

Clients::~Clients()
{
    clearClient();
}

bool Clients::addClient(QString name, QString pwd, QString ip, quint16 port, quint16 udpPort)
{
    if (!this->clientNodes.contains(name)) {
        ClientNode client;
        client.name = name;
        client.pwd = pwd;
        client.ip = ip;
        client.port = port;
        client.udpPort = udpPort;
        this->clientNodes[name] = client;

        return true;

    } else if (this->clientNodes[name].pwd == pwd) {
        this->clientNodes[name].ip = ip;
        this->clientNodes[name].port = port;
        this->clientNodes[name].udpPort = udpPort;

        return true;
    }

    return false;
}

bool Clients::removeClient(QString name, QString pwd)
{
    if (this->clientNodes.contains(name) && this->clientNodes[name].pwd == pwd) {
        this->clientNodes.remove(name);

        return true;
    }

    return false;
}


