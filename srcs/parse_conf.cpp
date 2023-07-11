
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
    _uploadPath = 0;
    _redirection.first = "";
    _redirection.second = "";
    _cgi.clear();
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
    _uploadPath = 0;
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

void locationParse(std::string &directive, std::istringstream &iss, Location &currentLocation) {
    size_t tmp;
    std::string value, value1;
    if (directive == "root" && iss >> value) {
        if (!currentLocation.getRoot().empty())
            throw std::invalid_argument("Invalid root directive");
        iss >> value1;
        if (!iss.fail())
            throw std::invalid_argument("Invalid root directive");
        currentLocation.setRoot(value);
    } else if (directive == "error_page") {
        std::vector<std::pair<int, std::string> > errorPage;
        while(iss >> tmp >> value)
            errorPage.push_back(std::make_pair(tmp, value));
        currentLocation.setError_pages(errorPage);
    } else if(directive == "cgi") {
        while(iss >> value >> value1)
        {
            if (value1 != ".py" && value1 != ".pl")
                throw std::invalid_argument("Invalid cgi extension: " + value1);
            if (currentLocation.get_cgi().size() >= 2 || value.empty() || value1.empty())
                throw std::invalid_argument("Invalid cgi directive");
            currentLocation.get_cgi().push_back(Cgi(value, value1));
        }
        if (iss.fail() && (!iss.eof() || currentLocation.get_cgi().back().get_Cgi().first != value))
            throw std::invalid_argument("Invalid cgi value: " + value);
        
    } else if (directive == "allow_methods") {
        while(iss >> value) {
            if ((value != "GET" && value != "POST" && value != "DELETE") || find(currentLocation.getAllowedMethods().begin(), currentLocation.getAllowedMethods().end(), value) != currentLocation.getAllowedMethods().end())
                throw std::invalid_argument("Invalid allow_methods directive");
            currentLocation.getAllowedMethods().push_back(value);
        }
    } else if (directive == "index") {
        while(iss >> value)
            currentLocation.getIndex().push_back(value);
    } else if(directive == "upload" && iss >> value) {
        iss >> value1;
        if (!iss.fail())
            throw std::invalid_argument("Invalid upload directive");
        if (value == "on")
            currentLocation.setUploadPath(1);
        else if (value == "off")
            currentLocation.setUploadPath(0);
        else
            throw std::invalid_argument("Invaalid upload value: " + value);
    } else if(directive == "return" && iss >> value >> value1) {
        iss >> value1;
        if (!iss.fail())
            throw std::invalid_argument("Invalid autoindex directive");
        if (value != "301" && value != "302")
            throw std::invalid_argument("Invalid return value: " + value);
        currentLocation.setRedirection(std::make_pair (value,value1));
    } else if(directive == "autoindex" && iss >> value) {
        iss >> value1;
        if (!iss.fail())
            throw std::invalid_argument("Invalid autoindex directive");
        if (value != "on" && value != "off")
            throw std::invalid_argument("Invalid autoindex value: " + value);
        currentLocation.setAutoIndex(value == "on" ? true : false);
    } else if (directive == "max_body_size" && iss >> tmp) {
        iss >> value1;
        if (!iss.fail())
            throw std::invalid_argument("Invalid max_body_size directive");
        currentLocation.setClientMaxBodySize(tmp);
    }
    else
        throw std::invalid_argument("Invalid directive: " + directive);
}

void Config::parse_config()
{
    std::string line;
    std::istringstream iss;
    std::string directive;
    std::string value, value1;
    Server currentServer;
    Location currentLocation;
    std::string defaultS;
    int open = 0;
    int s = 0;

    while (getline(this->_Configfile, line))
    {
        if (line.empty() || line[0] == '#' || line.find_first_not_of(" \t") == std::string::npos)
            continue;
        else
        {
            iss.str(line);
            if(!(iss >> directive))
                break;
            if (directive == "server")
            {
                if (s >= 1)
                    throw std::invalid_argument("Invalid server directive");
                s += 1;
            }
            else if (directive == "{")
                open += 1;
            else if (directive == "}") {
                if (open <= 0)
                    throw std::invalid_argument("Invalid closing bracket");
                else if (open == 1)
                {
                    if (this->_Servers[currentServer.getPort()].empty()){
                        this->_Servers[currentServer.getPort()].push_back(currentServer);
                    }
                    else if (currentServer.getDefault()) {
                        if (this->_Servers[currentServer.getPort()][0].getDefault() == true) {
                            throw std::invalid_argument("two default servers");
                        }
                        this->_Servers[currentServer.getPort()].push_back(this->_Servers[currentServer.getPort()][0]);
                        this->_Servers[currentServer.getPort()][0] = currentServer;
                    } else {
                        this->_Servers[currentServer.getPort()].push_back(currentServer);
                    }
                    currentServer = Server();
                    s = 0;
                }
                open -= 1;
            } else if (directive == "listen" && iss >> value) {
                if (currentServer.getPort() != 0)
                    throw std::invalid_argument("Invalid listen directive");
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
            } else if (directive == "server_name") {
                while(iss >> value)
                    currentServer.getServerNames().push_back(value);
            } else if (directive == "location") {
                if (iss >> value) {
                    currentLocation.setLocationPath(value);
                }
                else
                    throw std::invalid_argument("Invalid location directive");
                iss >> value1;
                if (!iss.fail())
                    throw std::invalid_argument("Invalid root directive");
                int o_tmp = open;
                open = 0;
                while (getline(this->_Configfile, line))
                {
                    if (line.empty() || line[0] == '#' || line.find_first_not_of(" \t") == std::string::npos)
                        continue;
                    else
                    {
                        iss.clear();
                        iss.str(line);

                        if(!(iss >> directive)) {
                            break;
                        }
                        if (directive == "{") {
                            open += 1;
                        } else if (directive == "}") {
                            if (open == 0)
                                throw std::invalid_argument("Invalid closing bracket");
                            else if (open == 1) {
                                if (currentLocation.getRoot().empty())
                                    throw std::invalid_argument("missing root directive");
                                currentServer.getLocations().push_back(currentLocation);
                                currentLocation = Location();
                                break;
                            }
                            open -= 1;
                        }
                        else {
                            try {
                                locationParse(directive, iss, currentLocation);
                            } catch (std::exception &e) {
                                throw std::invalid_argument(e.what());
                            }
                        }
                    }
                }
                open = o_tmp;
            }
            else {
                try {
                    locationParse(directive, iss, currentServer.getDefaultLocation());
                    // currentServer.getDefaultLocation().setLocationPath("/");
                } catch (std::exception &e) {
                    throw std::invalid_argument(e.what());
                }
            }
            iss.clear();
        }
    }
    if (open < 0 || open >= 1)
        throw std::invalid_argument("Invalid closing bracket");
    this->_Configfile.close();
}
