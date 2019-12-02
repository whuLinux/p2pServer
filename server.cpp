#include "server.h"

Server::Server()
{
    this->port = 8808;
    this->util = new UDPServerUtil(this->port);
}

Server::~Server()
{
    delete this->util;
    this->util = nullptr;
}


bool Server::startWorking()
{
    return this->util->stablishServer();
}

