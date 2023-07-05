
#include "../includes/response.hpp"
#include "../includes/server.hpp"
#define CRLF "\r\n"

Response::Response() : _offset(0), fd(0), is_open(0), _content_type(""), file(""), _resp(std::make_pair("", 0)), _status_code(0) {
    mime_types = mime_types_init();
    initErrorMessages();
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
    std::ifstream file("./tools/mime.types");
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string extension;
        std::string contentType;
        char a;
        if (iss >> contentType >> a >> extension)
            mimeTypes[extension] = contentType;
    }
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

void Response::HandleGet(Request &req, Location &loc, Server &server)
{
    int fdtmp = dup(0);
    int fdin[2];
    int fdout[2];
    pid_t child_pid;
    int rc;
    const int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    std::stringstream output;
    ssize_t bytesRead;


    std::map<std::string, std::string> mimeTypes = mime_types_init();
    std::string request_resource = loc.getRoot() + req.getPath() + req.getFile();
    std::string contentType = getContentType(request_resource, mimeTypes);
    std::vector<std::string>::iterator it;
    std::ifstream file;

    std::cout << request_resource << std::endl;
    if (!file_exists(request_resource.c_str())) {
        generateErrorPage(404);
        return ;
    }
    if (isDirectory(request_resource.c_str())) {
        if (request_resource[request_resource.length() - 1] != '/')
        {
            generateErrorPage(301);
            setHeader("Location", req.getPath() + req.getFile() + "/");
            return ;
        }
        for (it = loc.getIndex().begin(); it != loc.getIndex().end(); it++) {
            if (file_exists((request_resource + *it).c_str())) {
                request_resource += *it;
                break;
            }
        }
        if (it == loc.getIndex().end()) {
            if (loc.getAutoIndex()) {
                auto_indexing(request_resource.c_str());
                return ;
            } else {
                generateErrorPage(404);
                return ;
            }
        }
    }
    // check cgi
    // if (server.get_cgi().OK()) {
    //     // execute cgi
    //     server.get_cgi().initEnv(req, "localhost", loc.getRoot());
    //     pipe(fdin);
    //     pipe(fdout);
    //     child_pid = fork();
    //     if (child_pid < 0)
    //         exit(1);
    //     if (child_pid == 0) {
    //         dup2(fdout[1], STDOUT_FILENO);
    //         dup2(fdin[0], STDIN_FILENO);
    //         close(fdout[0]);
    //         close(fdin[1]);
    //         char *av[3];
    //         av[0] = strdup(server.get_cgi().get_Cgi().first.c_str());
    //         av[1] = strdup(request_resource.c_str());
    //         av[2] = NULL;
    //         execve(av[0], av, server.get_cgi().getEnv());
    //         exit(0);
    //     }
    //     close(fdout[1]);
    //     close(fdin[0]);
    //     rc = write(fdin[1], req.getBody().data(), req.getBody().size());
    //     if (rc != req.getBody().size()) {
    //         perror("error write: ");
    //         exit(1);
    //     }
    //     close(fdin[1]);
    //     while ((bytesRead = read(fdout[0], buffer, BUFFER_SIZE)) > 0)
    //         output.write(buffer, bytesRead);
    //     close (fdout[0]);
    //     wait (NULL);
    //     dup2(fdtmp, 0);
    //     setHeader("Status", "200 OK");
    //     setHeader("Content-Type", "text/html; charset=UTF-8");
    //     setHeader("Connection", "close");
    //     setHeader("Content-Length", std::to_string(output.str().length()));
    //     _resp.first += output.str();
    //     _resp.second = _resp.first.length();

    //     return;
    // }
    setHeader("Status", "200 OK");
    setHeader("Content-Type", contentType);
    file.open(request_resource, std::ios::binary | std::ios::ate);
    setHeader("Content-Length", std::to_string(file.tellg()));
    setHeader("Connection", "Keep-Alive");
    this->file = request_resource;
}

void Response::generateErrorPage(int code)
{
    auto it = errorMessages.find(code);
    if (it != errorMessages.end()) {
        std::string errorMessage = it->second;
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

const Location   & Response::match_loc(Server & server, std::string const & path)
{
    std::vector<Location>::iterator it = server.getLocations().begin();
    std::vector<Location>::iterator ite = server.getLocations().end();

    while (it != server.getLocations().end())
    {
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
        this->generateErrorPage(404);
        return Location(-1);
    } else if (it == server.getLocations().end() && ite != server.getLocations().end())
        it = ite;
    return *it;
}

void Response::HandlePost(Request &req, Location &loc, Server &server)
{
    std::string request_resource = loc.getRoot() + req.getPath() + req.getFile();
    int fdtmp = dup(0);
    int fdin[2];
    int fdout[2];
    pid_t child_pid;
    int rc;
    const int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    std::stringstream output;
    ssize_t bytesRead;
    Location tmp = match_loc(server, req.getHeaders().find("Referer")->second);

    if (loc.getClientMaxBodySize() < req.getBody().size()) {
        generateErrorPage(413);
        return ;
    }
    std::cout << "HMMM" << std::endl;
    if (tmp.get_a() == -1)
        return ;
    if (tmp.getUploadPath())
    {
        std::cout << "HMMM1111" << std::endl;
        server.get_cgi().initEnv(req, "localhost", loc.getRoot());
        pipe(fdin);
        pipe(fdout);
        child_pid = fork();
        if (child_pid < 0)
            exit(1);
        if (child_pid == 0) {
            dup2(fdout[1], STDOUT_FILENO);
            dup2(fdin[0], STDIN_FILENO);
            close(fdout[0]);
            close(fdin[1]);
            char *av[3];
            av[0] = strdup(server.get_cgi().get_Cgi().first.c_str());
            av[1] = strdup(request_resource.c_str());
            av[2] = NULL;
            execve(av[0], av, server.get_cgi().getEnv());
            exit(0);
        }
        close(fdout[1]);
        close(fdin[0]);
        rc = write(fdin[1], req.getBody().data(), req.getBody().size());
        if (rc != req.getBody().size()) {
            perror("error write: ");
            exit(1);
        }
        close(fdin[1]);
        while ((bytesRead = read(fdout[0], buffer, BUFFER_SIZE)) > 0)
            output.write(buffer, bytesRead);
        close (fdout[0]);
        wait (NULL);
        dup2(fdtmp, 0);
        setHeader("Status", "200 OK");
        setHeader("Content-Type", "text/html; charset=UTF-8");
        setHeader("Connection", "close");
        setHeader("Content-Length", std::to_string(output.str().length()));
        _resp.first += output.str();
        _resp.second = _resp.first.length();
    }
    else
        generateErrorPage(503);
    // std::vector<std::string>::iterator it;
    // if (isDirectory(request_resource.c_str()))
    // {
    //     if (request_resource[request_resource.length() - 1] != '/')
    //     {
    //         _resp.first = "HTTP/1.1 301 Moved Permanently\nLocation: " + req.getPath() + req.getFile() + "/\nContent-Type: text/html\nContent-Length: 13\n\n301 moved permanently";
    //         _resp.second = 84;
    //         return ;
    //     }
    //     for (it = loc.getIndex().begin(); it != loc.getIndex().end(); it++)
    //     {
    //         if (file_exists((request_resource + *it).c_str()))
    //         {
    //             if (!server.get_cgi().OK())
    //             {
    //                  // run_cgi();
    //                  return ;
    //             }
    //             else
    //             {
    //                 _resp.first = "HTTP/1.1 403 Forbidden\nContent-Type: text/html\nContent-Length: 13\n\n403 forbidden";
    //                 _resp.second = 84;
    //                 return ;
    //             }
    //         }
    //     }
    //      _resp.first = "HTTP/1.1 403 Forbidden\nContent-Type: text/html\nContent-Length: 13\n\n403 forbidden";
    //     _resp.second = 84;
    //     return ;
    // }
    return ;
}

void Response::HandleDelete(Request &req, Location &loc, Server &Server)
{
    std::string request_resource = loc.getRoot() + req.getPath() + req.getFile();
    if (!file_exists(request_resource.c_str())) {
        generateErrorPage(404);
    }
    else if (isDirectory(request_resource.c_str())) {
        if (request_resource[request_resource.length() - 1] != '/') {
            generateErrorPage(409);
        } else if (Server.get_cgi().OK()) {
            if(!remove(request_resource.c_str())) {
                generateErrorPage(204);
            } else {
                if(access(request_resource.c_str(), W_OK)) {
                    generateErrorPage(403);
                }
                else {
                    generateErrorPage(500);
                }
            }
        }
    }
    else {
       if(Server.get_cgi().OK()) {
            if(!remove(request_resource.c_str())) {
                generateErrorPage(204);
            } else {
                generateErrorPage(500);
            }
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
    for (const auto& pair : headers) {
        oss << pair.first << ": " << pair.second << "\r\n";
    }
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
    Location it = this->match_loc(server, req.getPath());
    if (_status_code != 0) {
        std::cout << "generate error page " << _status_code << std::endl;
        generateErrorPage(_status_code);
        return ;
    }
    if (it.get_a() == -1)
        return ;
    if (it.getRedirection().first != "") {
        generateErrorPage(301);
        setHeader("Location", it.getRedirection().first);
        return ;
    }
    if (std::find(it.getAllowedMethods().begin(), it.getAllowedMethods().end(), req.getMethod()) == it.getAllowedMethods().end()) {
        generateErrorPage(405);
        return ;
    }
    if (req.getMethod() == "GET") {
        HandleGet(req, it, server);
    } else if (req.getMethod() == "POST") {
        HandlePost(req, it, server);
    } else if (req.getMethod() == "DELETE") {
        HandleDelete(req, it, server);
    }
}

void Response::setHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}
