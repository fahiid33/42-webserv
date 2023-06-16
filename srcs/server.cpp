#include "../includes/server.hpp"

Server::Server()
{
    FD_ZERO(&master_set);
    FD_ZERO(&working_set);
    FD_ZERO(&write_fds);
    end_server = 0;
    max_sd = 0;
}

Server::~Server()
{
}

void Server::setMasterSet(int fd)
{
    FD_SET(fd, &master_set);
}

fd_set& Server::getMasterSet()
{
    return master_set;
}

void Server::setMaxSd(int sd)
{
    max_sd = sd;
}

int Server::getMaxSd()
{
    return max_sd;
}

void Server::setServerFd(int fd)
{
    ser.setSocket_fd(fd);
}

int Server::getServerFd()
{
    return ser.getSocket_fd();
}

Socket &Server::getServerSocket()
{
    return ser;
}

int Server::getEndServer()
{
    return end_server;
}

void Server::setEndServer(int end_server)
{
    this->end_server = end_server;
}

void Server::setServerSocket(Socket &sock)
{
    ser = sock;
}

fd_set& Server::getWriteFds()
{
    return write_fds;
}

fd_set& Server::getWorkingSet()
{
    return working_set;
}