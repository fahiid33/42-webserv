
#include "../includes/response.hpp"
#include "../includes/server.hpp"
#define CRLF "\r\n"

Response::Response() : _content_type(""), _status_code(0), fd(0), file(""), is_open(0), _resp(std::make_pair("", 0)), _offset(0) {
    mime_types = mime_types_init();
    initErrorMessages();
    headers.clear();
}

Response::~Response() {
    this->clear();
}

std::map<std::string, std::string> &Response::getHeaders()
{
    return headers;
}

Response::Response(const Response &resp)
{
    this->_offset = resp._offset;
    this->fd = resp.fd;
    this->is_open = resp.is_open;
    this->file = resp.file;
    this->_resp = resp._resp;
    this->_content_type = resp._content_type;
    this->mime_types = resp.mime_types;
    this->_status_code = resp._status_code;
    this->headers = resp.headers;
    this->errorMessages = resp.errorMessages;
}

Response & Response::operator=(const Response &resp)
{
    this->_offset = resp._offset;
    this->fd = resp.fd;
    this->is_open = resp.is_open;
    this->file = resp.file;
    this->_resp = resp._resp;
    this->mime_types = resp.mime_types;
    this->_content_type = resp._content_type;
    this->_status_code = resp._status_code;
    this->headers = resp.headers;
    this->errorMessages = resp.errorMessages;
    return *this;
}

void Response::clear()
{
    this->_offset = 0;
    this->fd = -1;
    this->is_open = 0;
    this->file = "";
    this->_resp = std::make_pair("", 0);
    this->_content_type = "";
    this->_status_code = 0;
    this->headers.clear();
}

size_t last_char_pos(std::string str, std::string str2)
{
    std::string::iterator it = str.begin();
    std::string::iterator it2 = str2.begin();
    size_t pos = 0;
    while (*it == *it2 && it != str.end() && it2 != str2.end())
    {
        it++;
        it2++;
        pos++;
    }
    return pos;
}

Response::Response(Request & req, Server & server)
{
    (void)server;
    (void)req;
}

u_long Response::getOffset()
{
    return _offset;
}

void Response::setOffset(u_long offset)
{
    _offset = offset;
}

int Response::getStatusCode()
{
    return _status_code;
}

void Response::setStatusCode(int status_code)
{
    _status_code = status_code;
}

int Response::getIsOpen()
{
    return is_open;
}

void Response::setIsOpen(int is_open)
{
    this->is_open = is_open;
}

void Response::setFd(int fd)
{
    this->fd = fd;
}

int Response::getFd()
{
    return fd;
}

std::string Response::getFile()
{
    return file;
}

void Response::setFile(std::string file)
{
    this->file = file;
}

std::pair<std::string, u_long> &Response::getResp()
{
    return _resp;
}

void Response::setResp(const std::pair<std::string, u_long> &resp)
{
    _resp = resp;
}

std::map<std::string, std::string>    Response::mime_types_init()
{
    std::map<std::string, std::string> mimeTypes;
    std::ifstream file;
    std::string line;

    file.open("./tools/mime.types");
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string extension;
        std::string contentType;
        char a;
        if (iss >> contentType >> a >> extension)
            mimeTypes[extension] = contentType;
    }
    file.close();
    return mimeTypes;
}

int isDirectory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

bool file_exists (const char *filename) {
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}

void    Response::auto_indexing(const char *dir)
{
    std::string str("<html><head><title>Index of /</title></head><body><h1>Index of /</h1><hr><pre>");
    DIR *dp;
    struct dirent *dirp;

    setHeader("Status", "200 OK");
    setHeader("Content-Type", "text/html");
    if ((dp = opendir(dir)) == NULL)
    {
        str += "</pre><hr></body></html>";
        setHeader("Content-Length", std::to_string(str.length()));
        _resp.first = str;
        _resp.second = str.length();
        return ;
    }
    while ((dirp = readdir(dp)) != NULL)
    {
        str += "<a href=\"";
        str += dirp->d_name;
        str += "\">";
        str += dirp->d_name;
        str += "</a>\n";
    }
    str += "</pre><hr></body></html>";
    setHeader("Content-Length", std::to_string(str.length()));
    _resp.first = str;
    _resp.second = str.length();
}

void Response::HandleGet(Request &req, Location &loc)
{
    std::map<std::string, std::string> mimeTypes = mime_types_init();
    std::string request_resource = loc.getRoot() + req.getPath() + req.getFile();
    std::string contentType = getContentType(request_resource, mimeTypes);
    std::vector<std::string>::iterator it;
    std::ifstream file;

    if (!file_exists(request_resource.c_str())) {
        generateErrorPage(404, loc);
        return ;
    }
    if (isDirectory(request_resource.c_str())) {
        if (request_resource[request_resource.length() - 1] != '/')
        {
            generateErrorPage(301, loc);
            setHeader("Location", req.getPath() + req.getFile() + "/");
            return ;
        }
        for (it = loc.getIndex().begin(); it != loc.getIndex().end(); it++) {
            if (file_exists((request_resource + *it).c_str())) {
                req.setFile(*it);
                request_resource += *it;
                break;
            }
        }
        if (it == loc.getIndex().end()) {
            if (loc.getAutoIndex()) {
                auto_indexing(request_resource.c_str());
                return ;
            } else {
                generateErrorPage(404, loc);
                return ;
            }
        }
    }
    if (access(request_resource.c_str(), R_OK) == -1) {
        generateErrorPage(403, loc);
        return ;
    }
    std::string ext("");
    std::string ext1("");

    if (!loc.get_cgi().empty()) {
        ext = (loc.get_cgi()[0].get_Cgi().second);
        if (loc.get_cgi().size() > 1)
            ext1 = (loc.get_cgi()[1].get_Cgi().second);
    }
    std::string reqext("");
    if (req.getFile().find_last_of(".") != std::string::npos)
        reqext = req.getFile().substr(req.getFile().find_last_of("."));
    

    if (loc.get_cgi().empty() || ((ext == "" || ext != reqext) && (ext1 == "" || ext1 != reqext))){
        setHeader("Status", "200 OK");
        setStatusCode(200);
        setHeader("Content-Type", contentType);
        file.open(request_resource, std::ios::binary | std::ios::ate);
        setHeader("Content-Length", std::to_string(file.tellg()));
        if (req.getHeaders().find("Cookie") == req.getHeaders().end())
            setHeader("Set-Cookie", "lala=pepe; Path=/");
        file.close();
        this->file = request_resource;
        return ;

    }
    else if (ext == reqext) {
        if (loc.get_cgi()[0].OK()) {
            loc.get_cgi()[0].runCgi(req, loc, *this);
            return ;
        }
    } else if (ext1 == reqext) {
        if (loc.get_cgi()[1].OK()) {
            loc.get_cgi()[1].runCgi(req, loc, *this);
            return ;
        }
    }
    generateErrorPage(503, loc);
    // check cgi
}

void Response::generateErrorPage(int code, Location const &loc)
{
    // loc.print_location();
    std::map<int, std::string>::iterator it = errorMessages.find(code);
    if (it != errorMessages.end()) {
        std::string errorMessage = it->second;
        if (!loc.getError_pages().empty()) {
            size_t pos = 0;
                for (; pos < loc.getError_pages().size(); pos++)
                {
                    if (loc.getError_pages()[pos].first == code)
                        break;
                }
            if (pos != loc.getError_pages().size()) {
                std::ifstream file;
                file.open(loc.getError_pages()[pos].second);
                if (file.is_open()) {
                    std::stringstream buffer;
                    buffer << file.rdbuf();
                    std::string str = buffer.str();
                    this->setHeader("Content-Length", std::to_string(str.length()));
                    this->setHeader("Status", std::to_string(code) + " " + errorMessage);
                    this->setHeader("Content-Type", "text/html");
                    _resp.first = str;
                    _resp.second = str.length();
                    file.close();
                    return ;
                }
            }
        }
        std::string errorPage = "<!DOCTYPE html>\n";
        errorPage += "<html>\n";
        errorPage += "<head>\n";
        errorPage += "<title>Error " + std::to_string(code) + ": " + errorMessage + "</title>\n";
        errorPage += "<style>\n";
        errorPage += "body {font-family: Arial, sans-serif; margin: 0; padding: 20px;}\n";
        errorPage += "h1 {font-size: 24px;}\n";
        errorPage += "p {font-size: 16px;}\n";
        errorPage += "</style>\n";
        errorPage += "</head>\n";
        errorPage += "<body>\n";
        errorPage += "<h1>Error " + std::to_string(code) + ": " + errorMessage + "</h1>\n";
        errorPage += "<p>" + errorMessage + "</p>\n";
        errorPage += "</body>\n";
        errorPage += "</html>";

        this->setHeader("Status", std::to_string(code) + " " + errorMessage);
        this->setHeader("Content-Type", "text/html");
        this->setHeader("Content-Length", std::to_string(errorPage.length()));
        _resp.first = errorPage;
        _resp.second = errorPage.length();
    }
    else {
        std::cout << "haadd zbya: " << code << std::endl;
        this->setHeader ("Status", "500 Internal Server Error");
        this->setHeader ("Content-Type", "text/html");
        _resp.first = "<!DOCTYPE html>\n";
        _resp.first += "<html>\n";
        _resp.first = "500 internal server error";
        _resp.first += "</html>\n";
        this->setHeader("Content-Length", std::to_string(_resp.first.length()));
        _resp.second = _resp.first.length();
    }
}

std::vector<Location>::iterator   Response::match_loc(Server & server, std::string const & path)
{
    std::vector<Location>::iterator it = server.getLocations().begin();
    std::vector<Location>::iterator ite = server.getLocations().end();

    while (it != server.getLocations().end()) {
        if (path.find(it->getLocationPath()) != std::string::npos) {
            if (it->getLocationPath() == "/") {
                ite = it;
                it++;
                continue ;
            } else
                break ;
        }
        it++;
    }
    if (it == server.getLocations().end() && ite == server.getLocations().end()) {
        if (server.getDefaultLocation().getRoot() != "") {
            return it;
        }
        else
            this->generateErrorPage(404, Location(-1));
        return it;
    } else if (it == server.getLocations().end() && ite != server.getLocations().end())
        it = ite;
    return it;
}

 
int	Response::write_file_in_path(Location &client, std::vector<unsigned char> content, std::string path)
{
	size_t index = path.find_last_of("/");
	std::string file_name = path.substr(index + 1);
	std::string folder_path = path.substr(0, index);

	std::string command = "mkdir -p " + folder_path;
	system(command.c_str());
	int write_fd = open(path.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (write_fd < 0)
	{
	    close(write_fd);
		generateErrorPage(500, client);
		return -1;
	}
    
    char *buf = new char[content.size() + 1];
    for (size_t i = 0; i < content.size(); i++)
        buf[i] = content[i];

	int r = write(write_fd, buf, content.size());
	delete [] buf;
    if (r < 0)
	{	
		generateErrorPage(500, client);
		close(write_fd);
		return -1;
	}
	close(write_fd);
	return 0;
}

void Response::HandlePost(Request &req, Location &loc)
{
    std::vector<std::string>::iterator it;
    std::string request_resource = loc.getRoot() + req.getPath() + req.getFile();

    if (isDirectory(request_resource.c_str())) {
            if (request_resource[request_resource.length() - 1] != '/')
                request_resource += "/";
            if(!loc.getIndex().empty()){
                req.setFile(loc.getIndex()[0]);
                request_resource += req.getFile();
            }
            else
            {
                generateErrorPage(403, loc);
                return ;
            }
    }
    std::string ext("");
    std::string ext1("");

    if (!loc.get_cgi().empty()) {
        ext = (loc.get_cgi()[0].get_Cgi().second);
        if (loc.get_cgi().size() > 1)
            ext1 = (loc.get_cgi()[1].get_Cgi().second);
    }
    std::string reqext("");
    if (req.getFile().find_last_of(".") != std::string::npos)
        reqext = req.getFile().substr(req.getFile().find_last_of("."));
    

    if (loc.getUploadPath() && (loc.get_cgi().empty() || ((ext == "" || ext != reqext) && (ext1 == "" || ext1 != reqext)))) {
        if (loc.getClientMaxBodySize() < req.getBody().size()) {
            generateErrorPage(413, loc);
            return ;
        }
        std::cout << "HHAHAHAHAHHA" << std::endl;
        this->write_file_in_path(loc, req.getBody(), request_resource);
        setHeader("Status", "201 Created");
        setHeader("Content-Type", "text/html");
        _resp.first = "<!DOCTYPE html>\n";
        _resp.first += "<html>\n";
        _resp.first += "<head>\n";
        _resp.first += "<title>Created</title>\n";
        _resp.first += "</head>\n";
        _resp.first += "<body>\n";
        _resp.first += "<h1>Created</h1>\n";
        _resp.first += "</body>\n";
        _resp.first += "</html>\n";
        setHeader("Content-Length", std::to_string(_resp.first.length()));
        _resp.second = _resp.first.length();
        return ;
    }

    else if (req.getFile().find(ext) + ext.length() == req.getFile().size()) {
        if (loc.get_cgi()[0].OK()) {
            loc.get_cgi()[0].runCgi(req, loc, *this);
            return ;
        }
    } else if (req.getFile().find(ext1) + ext1.length() == req.getFile().size()) {
        if (loc.get_cgi()[1].OK()) {
            loc.get_cgi()[1].runCgi(req, loc, *this);
            return ;
        }
    }
    generateErrorPage(503, loc);
}

int Response::deleteDirectory(const char *path) {
    struct dirent *entry;
    DIR *dir = opendir(path);

    if (dir == NULL) {
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char filePath[PATH_MAX];
        snprintf(filePath, PATH_MAX, "%s/%s", path, entry->d_name);

        struct stat st;
        if (lstat(filePath, &st) == -1) {
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            if (deleteDirectory(filePath) == -1) {
                continue;
            }
        } else {
            if (unlink(filePath) == -1) {
                continue;
            }
        }
    }
    closedir(dir);
    if (rmdir(path) == -1) {
        return -1;
    }

    return 0;
}

void Response::HandleDelete(Request &req, Location &loc)
{
    std::string request_resource = loc.getRoot() + req.getPath() + req.getFile();
    if (!file_exists(request_resource.c_str())) {
        generateErrorPage(404, loc);
    }
    else if (isDirectory(request_resource.c_str())) {
        if (request_resource[request_resource.length() - 1] != '/') {
               generateErrorPage(409, loc);
        }
        else {
            if(!deleteDirectory(request_resource.c_str())) {
                generateErrorPage(204, loc);
            } else {
                if(access(request_resource.c_str(), W_OK)) {
                    generateErrorPage(403, loc);
                }
                else {
                    generateErrorPage(500, loc);
                }
            }
        }
    }
    else {
        if(!remove(request_resource.c_str())) {
            generateErrorPage(204, loc);
        } else {
            generateErrorPage(500, loc);
        }
    }
}

std::string  Response::getContentType(const std::string& file , std::map<std::string, std::string>& mime_t)
{
    size_t dotPos = file.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string extension = file.substr(dotPos + 1);
        std::map<std::string, std::string>::iterator it = mime_t.find(extension);
        if (it != mime_t.end())
            return it->second;
        else
            return "octet-stream";
    }
    if (file == "")
        return "text/html";
    return "text/html";
}

std::string Response::toString() const {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << headers.at("Status") << "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
        oss << it->first << ": " << it->second << "\r\n";
    oss << "\r\n";
    oss << _resp.first;
    return oss.str();
}

void Response::initErrorMessages()
{
    errorMessages[400] = "Bad Request";
    errorMessages[414] = "Request-URI Too Large";
    errorMessages[505] = "HTTP Version Not Supported";
    errorMessages[501] = "Not Implemented";
    errorMessages[411] = "Length Required";
    errorMessages[405] = "Method Not Allowed";
    errorMessages[404] = "Not Found";
    errorMessages[403] = "Forbidden";
    errorMessages[301] = "Moved Permanently";
    errorMessages[302] = "Found";
    errorMessages[204] = "No Content";
    errorMessages[201] = "Created";
    errorMessages[200] = "OK";
    errorMessages[500] = "Internal Server Error";
    errorMessages[413] = "Payload Too Large";
    errorMessages[409] = "Conflict";
    errorMessages[408] = "Request Timeout";
    errorMessages[512] = "Bad Gateway";
    errorMessages[503] = "Service Unavailable";
}

void  Response::prepare_response(Request & req, Server & server)
{

    std::vector<Location>::iterator it = this->match_loc(server, req.getPath());
    Location loc;
    
    if (it == server.getLocations().end())
    {
        if (server.getDefaultLocation().getRoot() != "") {
            loc = server.getDefaultLocation();
        } else {
            generateErrorPage(403, *it);
            return ;
        }
    }

    else
        loc = *it;
    if (_status_code != 0) {
        generateErrorPage(_status_code, loc);
        return ;
    }
    if (loc.getRedirection().first != "") {
        generateErrorPage(std::stoi(loc.getRedirection().first), loc);
        setHeader("Location", loc.getRedirection().second);
        return ;
    }
    if (std::find(loc.getAllowedMethods().begin(), loc.getAllowedMethods().end(), req.getMethod()) == loc.getAllowedMethods().end()) {
        generateErrorPage(405, loc);
        return ;
    }
    if (req.getMethod() == "GET") {
        std::cout << "GET" << std::endl;
        HandleGet(req, loc);
    } else if (req.getMethod() == "POST") {
        HandlePost(req, loc);
    } else if (req.getMethod() == "DELETE") {
        HandleDelete(req, loc);
    }
}

void Response::setHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}
