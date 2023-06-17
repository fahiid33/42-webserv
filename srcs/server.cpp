
#include "../includes/server.hpp"

Server::Server()
{
    FD_ZERO(&master_set);
    FD_ZERO(&working_set);
    FD_ZERO(&write_fds);
    _port = 0;
    _host = "";
    _clientMaxBodySize = 0;
    _serverNames.clear();
    _locations.clear();
    _error_pages.clear();
    _index.clear();
    _root = "";
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


size_t &Server::getPort()
{
    return this->_port;
}

std::vector<std::string> &Server::getServerNames()
{
    return this->_serverNames;
}

std::vector<Location> &Server::getLocations()
{
    return this->_locations;
}

size_t &Server::getClientMaxBodySize()
{
    return this->_clientMaxBodySize;
}

std::string &Server::getHost()
{
    return this->_host;
}

std::vector<std::pair<size_t, std::string> > &Server::getError_pages()
{
    return this->_error_pages;
}

void Server::setPort(size_t port)
{
    this->_port = port;
}

void Server::setServerNames(std::vector<std::string> serverNames)
{
    this->_serverNames = serverNames;
}

void Server::setLocations(std::vector<Location> locations)
{
    this->_locations = locations;
}

void Server::setHost(std::string host)
{
    this->_host = host;
}

void Server::setClientMaxBodySize(size_t clientMaxBodySize)
{
    this->_clientMaxBodySize = clientMaxBodySize;
}

void Server::setError_pages(std::vector<std::pair<size_t, std::string> > error_pages)
{
    this->_error_pages = error_pages;
}

void Server::setRoot(std::string root)
{
    this->_root = root;
}

std::string &Server::getRoot()
{
    return this->_root;
}

std::vector<std::string> &Server::getIndex()
{
    return this->_index;
}

void Server::setIndex(std::vector<std::string> index)
{
    this->_index = index;
}