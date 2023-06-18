
#include "../includes/response.hpp"
#include "../includes/server.hpp"

Response::Response() : _offset(0) {}

Response::~Response() {}

Response::Response(Request & req, Server & server)
{
    std::vector<Location>::iterator it = server.getLocations().begin();
    while (it != server.getLocations().end())
    {
        if (req.getPath() == it->getLocationPath())
        {
            std::cout << req.getPath() << " == " <<it->getLocationPath() << std::endl;
            break;
        }
        it++;
    }
    if (it == server.getLocations().end())
    {
        _resp.first = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 13\n\n404 not found";
        _resp.second = 84;
        return ;
    }
    if (it->getRedirection().first != "")
    {
        _resp.first = "HTTP/1.1 301 Moved Permanently\nLocation: " + it->getRedirection().first + "\nContent-Type: text/html\nContent-Length: 13\n\n301 moved permanently";
        _resp.second = 84;
        return ;
    }
    if (std::find(it->getAllowedMethods().begin(), it->getAllowedMethods().end(), req.getMethod()) == it->getAllowedMethods().end())
    {
        _resp.first = "HTTP/1.1 405 Method Not Allowed\nContent-Type: text/html\nContent-Length: 13\n\n405 method not allowed";
        _resp.second = 84;
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
}


u_long Response::getOffset()
{
    return _offset;
}

void Response::setOffset(u_long offset)
{
    _offset = offset;
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
    std::string contentType = getContentType(req.getFile(), mimeTypes);
    std::vector<std::string>::iterator it;
                std::cout << "starrrrrt" << std::endl;

    _resp.first = "HTTP/1.1 200 OK\nContent-Type: ";
    _resp.first += contentType;
    std::ifstream file;
    std::string request_resource = loc.getRoot() + req.getPath() + req.getFile();
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
                std::cout << "starrrrrt   " << request_resource << std::endl;

                auto_indexing(request_resource.c_str());
                std::cout << "auto indexing" << _resp.first << "\n lenght :" << _resp.second << std::endl;
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
    file.open(request_resource, std::ios::binary | std::ios::ate);
    std::string str;
    _resp.first += "\nContent-Length:";
    _resp.second = file.tellg();
    file.seekg(0, std::ios::beg);
    _resp.first += std::to_string(_resp.second);
    _resp.first += "\n\n";
    _resp.second = _resp.second + _resp.first.length();
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    _resp.first += content;
}

void Response::HandlePost(Request &req, Location &loc)
{
    _resp.first = "HTTP/1.1 201 Created\nContent-Type: text/html\nContent-Length: 13\n\n201 created";
    _resp.second = 84;
}

void Response::HandleDelete(Request &req, Location &loc)
{
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
    return "application/octet-stream";
}

std::pair<std::string, u_long> prepare_response(Request & req, Server & server)
{
    Response respp(req, server);

    return respp.getResp();
}