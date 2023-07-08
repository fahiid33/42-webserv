
#include "../includes/parse_conf.hpp"

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
    _locationNumber = "";
    _locationPath = "";
    _clientMaxBodySize = 0;
    _allowedMethods.clear();
    _root = "";
    _index.clear();
    _autoIndex = false;
    _error_pages.clear();
    _uploadPath = "";
    _redirection.first = "";
    _redirection.second = "";
    _cgi = Cgi();
}

Location::~Location()
{
    _locationNumber = "";
    _locationPath = "";
    _clientMaxBodySize = 0;
    _allowedMethods.clear();
    _root = "";
    _index.clear();
    _autoIndex = false;
    _error_pages.clear();
    _uploadPath = "";
    _redirection.first = "";
    _redirection.second = "";
    _cgi.clear();
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

std::vector<std::string> &Location::getIndex()
{
    return this->_index;
}

bool &Location::getAutoIndex()
{
    return this->_autoIndex;
}

bool &Location::getUploadPath()
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

void Location::setUploadPath(bool uploadPath)
{
    this->_uploadPath = uploadPath;
}

void Location::setRedirection(std::pair<std::string, std::string> redirection)
{
    this->_redirection = redirection;
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
    std::string defaultS;

    while (getline(this->_Configfile, line))
    {
        if (line.empty() || line[0] == '#' || line.find_first_not_of(" \t") == std::string::npos)
            continue;
        else
        {
            
            iss.str(line);
            if(!(iss >> directive))
                break;
            if (directive == "listen" && iss >> value) {
                std::string defaultS;
                if (iss >> defaultS && defaultS != "default_server")
                    throw std::invalid_argument("Invalid default_server value: " + defaultS);
                else if (defaultS == "default_server")
                    currentServer.setDefault(true);
                std::size_t colonPos = value.find(':');
                if (colonPos == std::string::npos) {
                    std::string port = value;
                    if (port.find_first_not_of("0123456789") != std::string::npos)
                        throw std::invalid_argument("Invalid port value: " + port);
                    currentServer.setPort(std::stoi(port));
                } else {
                    std::string ip = value.substr(0, colonPos);
                    if (!ip.empty())
                        throw std::invalid_argument("don't support ip: " + ip);
                    std::string port = value.substr(colonPos + 1);
                    if (port.find_first_not_of("0123456789") != std::string::npos)
                        throw std::invalid_argument("Invalid port value: " + port);
                    currentServer.setPort(std::stoi(port));
                }
            } else if (directive == "root" && iss >> value) {
                currentServer.setRoot(value);
            } else if (directive == "server_name") {
                while(iss >> value)
                    currentServer.getServerNames().push_back(value);
            } else if (directive == "index") {
                while(iss >> value)
                    currentServer.getIndex().push_back(value);
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
                        } else if (directive == "root" && iss >> value) {
                            currentLocation.setRoot(value);
                        } else if (directive == "error_page") {
                            std::vector<std::pair<int, std::string> > errorPage;
                            while(iss >> tmp >> value)
                                errorPage.push_back(std::make_pair(tmp, value));
                            currentLocation.setError_pages(errorPage);
                        } else if(directive == "cgi") {
                            while(iss >> tmp >> value)
                                currentLocation.get_cgi().push_back(Cgi(tmp, value));
                        } else if (directive == "allow_methods") {
                            while(iss >> value)
                                currentLocation.getAllowedMethods().push_back(value);
                        } else if (directive == "index") {
                            while(iss >> value)
                                currentLocation.getIndex().push_back(value);
                        } else if(directive == "upload" && iss >> value) {
                            if (value == "on")
                                currentLocation.setUploadPath(1);
                            else if (value == "off")
                                currentLocation.setUploadPath(0);
                            else
                                throw std::invalid_argument("Invaalid upload value: " + value);
                        } else if(directive == "return" && iss >> value >> value1) {
                            currentLocation.setRedirection(std::make_pair (value,value1));
                        } else if(directive == "autoindex" && iss >> value) {
                            currentLocation.setAutoIndex(value == "on" ? true : false);
                        } else if (directive == "max_body_size" && iss >> tmp) {
                                currentLocation.setClientMaxBodySize(tmp);
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
                if (currentServer.getDefault()) {
                    if (!this->_Servers.empty() && this->_Servers[currentServer.getPort()][0].getDefault() == true) {
                        throw std::invalid_argument("two default servers");
                    }
                    this->_Servers[currentServer.getPort()].push_back(this->_Servers[currentServer.getPort()][0]);
                    this->_Servers[currentServer.getPort()][0] = currentServer;
                } else {
                    this->_Servers[currentServer.getPort()].push_back(currentServer);
                }
                currentServer = Server();
            }
            iss.clear();
        }
        
    }
    this->_Configfile.close();
}
