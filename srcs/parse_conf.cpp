
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
    _uploadPath = "";
    _redirection.first = "";
    _redirection.second = "";
    // _cgi_extension.clear();
    // _cgi_path.clear();

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
            } else if (directive == "root" && iss >> value) {
                currentServer.setRoot(value);
            } else if (directive == "host" && iss >> value) {
                currentServer.setHost(value);
            } else if (directive == "server_name") {
                while(iss >> value)
                    currentServer.getServerNames().push_back(value);
            } else if (directive == "index") {
                while(iss >> value)
                    currentServer.getIndex().push_back(value);
            } else if(directive == "cgi" && iss >> value >> value1) {
                    currentServer.get_cgi().set_Cgi(std::make_pair(value, value1));
            } else if (directive == "error_page") {
                std::vector<std::pair<size_t, std::string> > errorPage;
                while(iss >> tmp >> value)
                    errorPage.push_back(std::make_pair(tmp, value));
                currentServer.setError_pages(errorPage);
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
                this->_Servers.push_back(currentServer);
                currentServer = Server();
            }
            iss.clear();
        }
        
    }
    this->_Configfile.close();
}
