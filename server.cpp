#include "server.h"

/**
 * @brief The Server class
 * @author 余宗宪
 * P2P通信模块的服务器
 */

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

