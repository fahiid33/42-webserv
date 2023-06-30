
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
    std::string resp = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length:";
    std::string str = "<html><head><title>Index of /</title></head><body><h1>Index of /</h1><hr><pre>";
    DIR *dp;
    struct dirent *dirp;

    if ((dp = opendir(dir)) == NULL)
    {
        str += "</pre><hr></body></html>";
        resp += std::to_string(str.length());
        resp += "\n\n";
        resp += str;
        _resp.first = resp;
        _resp.second = resp.length();
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
    resp += std::to_string(str.length());
    resp += "\n\n";
    resp += str;
    _resp.first = resp;
    _resp.second = resp.length();
}

void Response::HandleGet(Request &req, Location &loc, Server &server)
{
    std::map<std::string, std::string> mimeTypes = mime_types_init();
    std::string request_resource = loc.getRoot() + req.getPath() + req.getFile();
    std::string contentType = getContentType(request_resource, mimeTypes);
    std::vector<std::string>::iterator it;

    // _resp.first = "HTTP/1.1 200 OK\nContent-Type: ";
    // _resp.first += contentType;
    setHeader("Status", "200 OK");
    setHeader("Content-Type", contentType);
    std::ifstream file;
    // std::cout << "request_resource: " << request_resource << std::endl;
    if (!file_exists(request_resource.c_str()))
    {
        generateErrorPage(404);
        return ;
    }
    if (isDirectory(request_resource.c_str()))
    {
        // check if request_resource has / at the end
        if (request_resource[request_resource.length() - 1] != '/')
        {
            // should update the page header to redirect to the same page with / at the end
            generateErrorPage(301);
            return ;
            // _resp.first = "HTTP/1.1 403 Forbidden\nContent-Type: text/html\nContent-Length: 13\n\n403 forbidden";
            // _resp.second = 84;
            // return ;
        }
        // check if request_resource has index file
        for (it = loc.getIndex().begin(); it != loc.getIndex().end(); it++)
        {
            //  /Users/hlachkar/www/pos/index.html
            if (file_exists((request_resource + *it).c_str()))
            {
                request_resource += *it;
                break;
            }
        }
        if (it == loc.getIndex().end())
        {
            if (loc.getAutoIndex())
            {
                auto_indexing(request_resource.c_str());
                return ;
            }
            else
            {
                generateErrorPage(403);
                return ;
            }
        }
    }
    file.open(request_resource, std::ios::binary | std::ios::ate);
    setHeader("Content-Length", std::to_string(file.tellg()));
    setHeader("Connection", "Keep-Alive");
    this->file = request_resource;
    // std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    // _resp.first += content + "\n";
}

void Response::HandlePost(Request &req, Location &loc, Server &server)
{
    // if location support upload
    if (loc.getClientMaxBodySize() < req.getBody().size())
    {
        _resp.first = "HTTP/1.1 413 Payload Too Large\nContent-Type: text/html\nContent-Length: 13\n\n413 payload too large";
        _resp.second = 84;
        return ;
    }
    std::string request_resource = loc.getRoot() + req.getPath() + req.getFile();

    server.get_cgi().initEnv(req, "localhost", loc.getRoot());

    _resp.first = "HTTP/1.1 200 OK" CRLF "Connection: close" CRLF
    "Content-Type: text/html; charset=UTF-8" CRLF;    
    int fdtmp = dup(0);


    // open a tmp file and put req.getBody() in it


    int fdin[2];
    int fdout[2];
    pipe(fdin);
    pipe(fdout);

    pid_t child_pid = fork();
    if (child_pid < 0) {
        exit(1);
    }

    if (child_pid == 0) {
        // Redirect the child process's STDOUT to write into the input pipe
        dup2(fdout[1], STDOUT_FILENO);

        // Redirect the child process's STDIN to read from the output pipe
        dup2(fdin[0], STDIN_FILENO);

        // Close unused pipe ends
        close(fdout[0]);
        close(fdin[1]);

        char *av[3];
        av[0] = strdup(server.get_cgi().get_Cgi().first.c_str());
        av[1] = strdup(request_resource.c_str());
        av[2] = NULL;

        execve(av[0], av, server.get_cgi().getEnv());

        exit(0);
    }

    // Close unused pipe ends
    close(fdout[1]);
    close(fdin[0]);

    // Write the POST body to the input pipe
    
    int rc = write(fdin[1], req.getBody().data(), req.getBody().size());
    if (rc != req.getBody().size()) {
        perror("error write: ");
        exit(1);
    }

    // Close the input pipe write end to indicate end of data
    close(fdin[1]);

    /* Read from the pipe and set resp */
    const int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    std::stringstream output;
    ssize_t bytesRead;
    while ((bytesRead = read(fdout[0], buffer, BUFFER_SIZE)) > 0)
    {
        output.write(buffer, bytesRead);
    }
    close (fdout[0]);
    wait (NULL);
    dup2(fdtmp, 0);

    _resp.first += "Content-Length: " + std::to_string(output.str().length()) + CRLF CRLF;

    _resp.first += output.str();
    _resp.second = _resp.first.length();

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
    if (!file_exists(request_resource.c_str()))
    {
        _resp.first = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 13\n\n404 not found";
        _resp.second = 84;
        return ;
    }
    if (isDirectory(request_resource.c_str()))
    {
        if (request_resource[request_resource.length() - 1] != '/')
        {
            _resp.first = "HTTP/1.1 409 Conflict\nContent-Type: text/html\nContent-Length: 13\n\n409 conflict";
            _resp.second = 82;
            return ;
        }
        if(Server.get_cgi().OK())
        {
            if(!remove(request_resource.c_str()))
            {
                _resp.first = "HTTP/1.1 204 No Content\nContent-Type: text/html\nContent-Length: 13\n\n204 no content";
                _resp.second = 84;
                return ;
            }
            else
            {
                if(access(request_resource.c_str(), W_OK))
                {
                    _resp.first = "HTTP/1.1 403 Forbidden\nContent-Type: text/html\nContent-Length: 13\n\n403 forbidden";
                    _resp.second = 84;
                    return ;
                }
                else
                {
                    _resp.first = "HTTP/1.1 500 Internal Server Error\nContent-Type: text/html\nContent-Length: 13\n\n500 internal server error";
                    _resp.second = 108;
                    return ;
                }
            }
        }
    }
    else
    {
       if(Server.get_cgi().OK())
       {
            if(!remove(request_resource.c_str()))
            {
                _resp.first = "HTTP/1.1 204 No Content\nContent-Type: text/html\nContent-Length: 13\n\n204 no content";
                _resp.second = 84;
                return ;
            }
            else
            {
                _resp.first = "HTTP/1.1 500 Internal Server Error\nContent-Type: text/html\nContent-Length: 13\n\n500 internal server error";
                _resp.second = 108;
                return ;
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
        _resp.first = errorPage;
    }
    else {
        this->setHeader ("Status", "500 Internal Server Error");
        this->setHeader ("Content-Type", "text/html");
        _resp.first = "<!DOCTYPE html>\n";
        _resp.first += "<html>\n";
        _resp.first = "500 internal server error";
        _resp.first += "</html>\n";
    }
}

void Response::initErrorMessages()
{
    errorMessages[400] = "Bad Request";
    errorMessages[414] = "Request-URI Too Large";
    errorMessages[505] = "HTTP Version Not Supported";
    errorMessages[501] = "Not Implemented";
    errorMessages[411] = "Length Required";
}

void  Response::prepare_response(Request & req, Server & server)
{
    std::vector<Location>::iterator it = server.getLocations().begin();
    std::vector<Location>::iterator ite = server.getLocations().end();

    if (_status_code != 0)
    {
        std::cout << "generate error page " << _status_code << std::endl;
        generateErrorPage(_status_code);
        return ;
    }

    while (it != server.getLocations().end())
    {
        if (req.getPath().find(it->getLocationPath()) != std::string::npos)
        {
            if (it->getLocationPath() == "/")
            {
                ite = it;
                it++;
                continue ;
            }
            else
                break ;
        }
        it++;
    }
    if (it == server.getLocations().end() && ite == server.getLocations().end())
    {
        _resp.first = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 13\n\n404 not found";
        _resp.second = 84;
        return ;
    }
    else if (it == server.getLocations().end() && ite != server.getLocations().end())
        it = ite;
    if (it->getRedirection().first != "")
    {
        std::cout << "redirection" << std::endl;
        _resp.first = "HTTP/1.1 301 Moved Permanently\nLocation: " + it->getRedirection().first + "\nContent-Type: text/html\nContent-Length: 13\n\n301 moved permanently";
        _resp.second = 84;
        return ;
    }
    if (std::find(it->getAllowedMethods().begin(), it->getAllowedMethods().end(), req.getMethod()) == it->getAllowedMethods().end())
    {
        std::cout << "redirection" << std::endl;
        _resp.first = "HTTP/1.1 405 Method Not Allowed\nContent-Type: text/html\nContent-Length: 13\n\n405 method not allowed";
        _resp.second = 102;
        return ;
    }
    if (req.getMethod() == "GET")
    {

        HandleGet(req, *it, server);
    }
    else if (req.getMethod() == "POST")
    {
        HandlePost(req, *it, server);
    }
    else if (req.getMethod() == "DELETE")
    {
        HandleDelete(req, *it, server);
    }
}

void Response::setHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}
