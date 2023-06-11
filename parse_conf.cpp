#include "parse_conf.hpp"

Config::Config() : _FilePath("conf/default.conf")
{
    this->_Configfile.open(this->_FilePath.c_str());
    if (this->_Configfile.is_open())
    {
        std::cout << "File is open" << std::endl;
    }
    else
        std::cout << "Unable to open file" << std::endl;
}

Config::Config(std::string FilePath) : _FilePath(FilePath)
{
    this->_Configfile.open(this->_FilePath.c_str());
    if (this->_Configfile.is_open())
    {
        std::cout << "File is open" << std::endl;
    }
    else
        std::cout << "Unable to open file" << std::endl;
}

Config::~Config()
{
}

Location::Location()
{
}

Location::~Location()
{
}

std::string &Location::getLocationNumber()
{
    return this->_locationNumber;
}

std::string &Location::getLocationPath()
{
    return this->_locationPath;
}

std::vector<std::string> &Location::getAllowedMethods()
{
    return this->_allowedMethods;
}

std::string &Location::getRoot()
{
    return this->_root;
}

std::string &Location::getClientMaxBodySize()
{
    return this->_clientMaxBodySize;
}

std::vector<std::string> &Location::getIndex()
{
    return this->_index;
}

std::string &Location::getAutoIndex()
{
    return this->_autoIndex;
}

std::string &Location::getUploadPath()
{
    return this->_uploadPath;
}

std::pair<std::string, std::string> &Location::getRedirection()
{
    return this->_redirection;
}

void Location::setLocationNumber(std::string locationNumber)
{
}

void Location::setLocationPath(std::string locationPath)
{
}

void Location::setAllowedMethods(std::vector<std::string> allowedMethods)
{
}

void Location::setRoot(std::string root)
{
}

void Location::setClientMaxBodySize(std::string clientMaxBodySize)
{
}

void Location::setIndex(std::vector<std::string> index)
{
}

void Location::setAutoIndex(std::string autoIndex)
{
}

void Location::setUploadPath(std::string uploadPath)
{
}

void Location::setRedirection(std::pair<std::string, std::string> redirection)
{
}

Socket::Socket()
{
}

Socket::~Socket()
{
}

int Socket::getSocket_fd()
{
    return this->_socket_fd;
}

struct sockaddr_in  *Socket::getSockaddr()
{
    return this->_sockaddr;
}

void Socket::setSocket_fd(int socket_fd)
{
}

void Socket::setSockaddr(struct sockaddr_in  *sockaddr)
{
}

Server::Server()
{
}

Server::~Server()
{
}

Socket &Server::getSocket()
{
    return this->_socket;
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

std::vector<std::pair<std::string, std::string> > &Server::getError_pages()
{
    return this->_error_pages;
}

std::string &Server::getUploadPath()
{
    return this->_uploadPath;
}

std::vector<std::string> &Server::getMime_types()
{
    return this->_mime_types;
}

void Server::setSocket(Socket socket)
{
}

void Server::setPort(size_t port)
{
}

void Server::setServerNames(std::vector<std::string> serverNames)
{
}

void Server::setLocations(std::vector<Location> locations)
{
}

void Server::setError_pages(std::vector<std::pair<std::string, std::string> > error_pages)
{
}

void Server::setUploadPath(std::string uploadPath)
{
}

void Server::setMime_types(std::vector<std::string> mime_types)
{
}


void Config::parse_conf()
{
    std::string line;
    std::istringstream iss;

    while (getline(this->_Configfile, line))
    {
        if (line.empty() || line[0] == '#' || line.find_first_not_of(" \t") == std::string::npos)
            continue;
        else
        {
            
        }
    }
    this->_Configfile.close();
}
