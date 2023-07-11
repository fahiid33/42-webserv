
#include "../includes/server.hpp"

Server::Server()
{
    FD_ZERO(&master_set);
    FD_ZERO(&working_set);
    FD_ZERO(&write_fds);
    _port = 0;
    _ip = "";
    _serverNames.clear();
    _locations.clear();
    _default = false;
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

fd_set &Server::getWriteCpy()
{
    return write_cpy;
}

fd_set &Server::getWorkingSet()
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

size_t &Location::getClientMaxBodySize()
{
    return this->_clientMaxBodySize;
}

std::string &Server::getIp()
{
    return this->_ip;
}

std::vector<std::pair<int, std::string> > Location::getError_pages() const
{
    return this->_error_pages;
}

void Server::setPort(size_t port)
{
    this->_port = port;
}

void Location::set_cgi(std::vector< Cgi >  cgi)
{
    this->_cgi = cgi;
}

void Server::setDefault(bool Default){
    this->_default = Default;
}

bool &Server::getDefault()
{
    return this->_default;
}

void Server::setServerNames(std::vector<std::string> serverNames)
{
    this->_serverNames = serverNames;
}

void Server::setLocations(std::vector<Location> locations)
{
    this->_locations = locations;
}

void Server::setIp(std::string ip)
{
    this->_ip = ip;
}

void Location::setClientMaxBodySize(size_t clientMaxBodySize)
{
    this->_clientMaxBodySize = clientMaxBodySize;
}

void Location::setError_pages(std::vector<std::pair<int, std::string> > error_pages)
{
    this->_error_pages = error_pages;
}

std::vector< Cgi > & Location::get_cgi()
{
    return this->_cgi;
}

Location &Server::getDefaultLocation()
{
    return this->l_default;
}

void Server::setDefaultLocation(Location &defautLocation)
{
    this->l_default = defautLocation;
}

Location &Location::operator=(Location const &rhs)
{
    if (this != &rhs)
    {
        this->_locationNumber = rhs._locationNumber;
        this->_locationPath = rhs._locationPath;
        this->_clientMaxBodySize = rhs._clientMaxBodySize;
        this->_error_pages = rhs._error_pages;
        this->_cgi = rhs._cgi;
        this->_root = rhs._root;
        this->_index = rhs._index;
        this->_autoIndex = rhs._autoIndex;
        this->_allowedMethods = rhs._allowedMethods;
        this->_uploadPath = rhs._uploadPath;
        this->_redirection = rhs._redirection;
    }
    return *this;
}