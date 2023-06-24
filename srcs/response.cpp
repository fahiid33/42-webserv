
#include "../includes/response.hpp"
#include "../includes/server.hpp"

Response::Response() : _offset(0), fd(0), is_open(0), _content_type(""), file(""), _resp(std::make_pair("", 0)) {
    mime_types = mime_types_init();
}

Response::~Response() {
    this->clear();
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

void Response::HandleGet(Request &req, Location &loc)
{
    std::map<std::string, std::string> mimeTypes = mime_types_init();
    std::string request_resource = loc.getRoot() + req.getPath() + req.getFile();
    std::string contentType = getContentType(request_resource, mimeTypes);
    std::vector<std::string>::iterator it;

    _resp.first = "HTTP/1.1 200 OK\nContent-Type: ";
    _resp.first += contentType;
    std::ifstream file;
    // std::cout << "request_resource: " << request_resource << std::endl;
    if (!file_exists(request_resource.c_str()))
    {
        _resp.first = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 13\n\n404 not found";
        _resp.second = 84;
        return ;
    }
    if (isDirectory(request_resource.c_str()))
    {
        // check if request_resource has / at the end
        if (request_resource[request_resource.length() - 1] != '/')
        {
            _resp.first = "HTTP/1.1 301 Moved Permanently\nLocation: " + req.getPath() + req.getFile() + "/\nContent-Type: text/html\nContent-Length: 13\n\n301 moved permanently";
            _resp.second = 84;
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
                _resp.first = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 13\n\n404 Not Found";
                _resp.second = 84;
                return ;
            }
        }
    }
    file.open(request_resource, std::ios::binary | std::ios::ate);
    std::string str;
    _resp.first += "\nConnection: Keep-Alive";
    _resp.first += "\nContent-Length: ";
    _resp.second = file.tellg();
    file.seekg(0, std::ios::beg);
    _resp.first += std::to_string(_resp.second);
    _resp.first += "\n\n";
    this->file = request_resource;
    // std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    // _resp.first += content + "\n";
}

void Response::HandlePost(Request &req, Location &loc)
{
    // if location support upload
    if (loc.getClientMaxBodySize() < req.getBody().length())
    {
        _resp.first = "HTTP/1.1 413 Payload Too Large\nContent-Type: text/html\nContent-Length: 13\n\n413 payload too large";
        _resp.second = 84;
        return ;
    }
    // 201 created
    std::string request_resource = loc.getRoot() + req.getPath() + req.getFile();
    
    std::string new_file(request_resource);
    // create the file if not exist
    open(new_file.c_str(), O_CREAT | O_WRONLY, 0777);

    std::ofstream out(new_file, std::ios::out | std::ios::binary);
    out << req.getBody();
    out.close();

    _resp.first = "HTTP/1.1 201 Created\nContent-Type: text/plain\nConnection: close\nContent-Length: 13\n\n201 created";
    _resp.second = 84;
    // out << in.rdbuf();
    // in.close();
    // out.close();
    // std::cout << "here" << std::endl;
    // remove(req.get_body().c_str());
    // just doing some tests khliha commented
    
    // if (file_exists(request_resource.c_str()))
    // {
    //     _resp.first = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 13\n\n404 not found";
    //     _resp.second = 84;
    //     return ;
    // }
    std::vector<std::string>::iterator it;
    if (isDirectory(request_resource.c_str()))
    {
        if (request_resource[request_resource.length() - 1] != '/')
        {
            _resp.first = "HTTP/1.1 301 Moved Permanently\nLocation: " + req.getPath() + req.getFile() + "/\nContent-Type: text/html\nContent-Length: 13\n\n301 moved permanently";
            _resp.second = 84;
            return ;
        }
        for (it = loc.getIndex().begin(); it != loc.getIndex().end(); it++)
        {
            if (file_exists((request_resource + *it).c_str()))
            {
                if (!loc.get_cgi_path().empty())
                {
                     // run_cgi();
                     return ;
                }
                else
                {
                    _resp.first = "HTTP/1.1 403 Forbidden\nContent-Type: text/html\nContent-Length: 13\n\n403 forbidden";
                    _resp.second = 84;
                    return ;
                }
            }
        }
         _resp.first = "HTTP/1.1 403 Forbidden\nContent-Type: text/html\nContent-Length: 13\n\n403 forbidden";
        _resp.second = 84;
        return ;
    }

}

void Response::HandleDelete(Request &req, Location &loc)
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
        if(loc.get_cgi_path().empty())
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
       if(loc.get_cgi_path().empty())
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
        if (it != mime_t.end()) {
            return it->second;
        }
    }
    if (file == "")
        return "text/html";
    return "text/html";
}

void  Response::prepare_response(Request & req, Server & server)
{
    std::vector<Location>::iterator it = server.getLocations().begin();
    std::vector<Location>::iterator ite = server.getLocations().end();


    // /gym/   ite = /     it = /gym/
    // /       ite = /     it = it.end()
    // /srcs   ite = /     it = it.end()
    // /       ite = ite.end()     it = it.end()

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
        std::cout << "response header : " << this->_resp.first << "00" << std::endl;
        return ;
    }
    else if (it == server.getLocations().end() && ite != server.getLocations().end())
        it = ite;
    // std::cout << "\033[33m" << "location = " << it->getLocationPath() << " for req path " << req.getPath() << std::endl;
    if (it->getRedirection().first != "")
    {
        _resp.first = "HTTP/1.1 301 Moved Permanently\nLocation: " + it->getRedirection().first + "\nContent-Type: text/html\nContent-Length: 13\n\n301 moved permanently";
        _resp.second = 84;
        std::cout << "response header : " << this->_resp.first << "00" << std::endl;
        return ;
    }
    if (std::find(it->getAllowedMethods().begin(), it->getAllowedMethods().end(), req.getMethod()) == it->getAllowedMethods().end())
    {
        _resp.first = "HTTP/1.1 405 Method Not Allowed\nContent-Type: text/html\nContent-Length: 13\n\n405 method not allowed";
        _resp.second = 102;
        std::cout << "response header : " << this->_resp.first << "00" << std::endl;
        return ;
    }
    if (req.getMethod() == "GET")
    {
        HandleGet(req, *it);
    }
    else if (req.getMethod() == "POST")
    {
        HandlePost(req, *it);
    }
    else if (req.getMethod() == "DELETE")
    {
        HandleDelete(req, *it);
    }
    std::cout << "response header : " << this->_resp.first << "00" << std::endl;
}