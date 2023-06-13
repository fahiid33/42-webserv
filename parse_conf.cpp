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

std::vector<std::string> &Location::get_cgi_path()
{
    return this->_cgi_path;
}

std::vector<std::string> &Location::get_cgi_extension()
{
    return this->_cgi_extension;
}

std::vector<std::string> &Location::getIndex()
{
    return this->_index;
}

bool &Location::getAutoIndex()
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
    this->_locationNumber = locationNumber;
}

void Location::setLocationPath(std::string locationPath)
{
    this->_locationPath = locationPath;
}

void Location::setAllowedMethods(std::vector<std::string> allowedMethods)
{
    this->_allowedMethods = allowedMethods;
}

void Location::setRoot(std::string root)
{
    this->_root = root;
}

void Location::setIndex(std::vector<std::string> index)
{
    this->_index = index;
}

void Location::setAutoIndex(bool autoIndex)
{
    this->_autoIndex = autoIndex;
}

void Location::setUploadPath(std::string uploadPath)
{
    this->_uploadPath = uploadPath;
}

void Location::setRedirection(std::pair<std::string, std::string> redirection)
{
    this->_redirection = redirection;
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

std::vector<std::string> &Server::getMime_types()
{
    return this->_mime_types;
}

void Server::setSocket(Socket socket)
{
    this->_socket = socket;
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

void Server::setMime_types(std::vector<std::string> mime_types)
{
    this->_mime_types = mime_types;
}


void Config::parse_config()
{
    std::string line;
    std::istringstream iss;
    std::string directive;
    std::string value, value1;
    size_t tmp;
    Server currentServer;
    Location currentLocation;

    while (getline(this->_Configfile, line))
    {
        if (line.empty() || line[0] == '#' || line.find_first_not_of(" \t") == std::string::npos)
            continue;
        else
        {
            
            iss.str(line);
            if(!(iss >> directive))
                break;
            if (directive == "listen" && iss >> tmp) {
                currentServer.setPort(tmp);
            } else if (directive == "host" && iss >> value) {
                currentServer.setHost(value);
            } else if (directive == "server_name") {
                while(iss >> value)
                    currentServer.getServerNames().push_back(value);
            } else if (directive == "error_page") {
                std::vector<std::pair<size_t, std::string> > errorPage;
                while(iss >> tmp >> value)
                    errorPage.push_back(std::make_pair(tmp, value));
                currentServer.setError_pages(errorPage);
            } else if (directive == "client_max_body_size" && iss >> tmp) {
                currentServer.setClientMaxBodySize(tmp);
            } else if (directive == "mime_type") {
                while(iss >> value)
                {
                    currentServer.getMime_types().push_back(value);
                }
            } else if (directive == "location"){
                if (iss >> value) {
                    currentLocation.setLocationPath(value);
                }
                while (getline(this->_Configfile, line))
                {
                    if (line.empty() || line[0] == '#' || line.find_first_not_of(" \t") == std::string::npos)
                        continue;
                    else
                    {
                        iss.clear();
                        iss.str(line);
                        // Parse location-specific directives
                        if(!(iss >> directive)) {
                            break;
                        }
                        else if (directive == "root" && iss >> value) {
                            currentLocation.setRoot(value);
                        } else if (directive == "allow_methods") {
                            while(iss >> value)
                                currentLocation.getAllowedMethods().push_back(value);
                        } else if (directive == "index") {
                            while(iss >> value)
                                currentLocation.getIndex().push_back(value);
                        } else if(directive == "upload_path" && iss >> value) {
                            currentLocation.setUploadPath(value);
                        } else if(directive == "return" && iss >> value >> value1) {
                            currentLocation.setRedirection(std::make_pair (value,value1));
                        } else if(directive == "autoindex" && iss >> value) {
                            currentLocation.setAutoIndex(value == "on" ? true : false);
                        } else if(directive == "cgi_path") {
                            while(iss >> value)
                                currentLocation.get_cgi_path().push_back(value);
                        } else if(directive == "cgi_ext") {
                            while(iss >> value)
                                currentLocation.get_cgi_extension().push_back(value);
                        }
                        else if (directive == "}") {
                            currentServer.getLocations().push_back(currentLocation);
                            currentLocation = Location();
                            break;
                        }
                    }
                }
            }
            else if (directive == "}") {
                this->_Servers.push_back(currentServer);
                currentServer = Server();
            }
            iss.clear();
        }
        
    }
    std::cout << _Servers.size() << std::endl;
    this->_Configfile.close();
}
