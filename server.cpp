#include "server.h"

Server::Server()
{
    this->port = 8080;
    this->util = new UDPServerUtil(this->port);
}

Server::~Server()
{
    delete this->util;
    this->util = nullptr;
}


bool Server::startWorking()
{
    this->port++;
    this->util->setPort(this->port);
    return this->util->stablishServer();
}

