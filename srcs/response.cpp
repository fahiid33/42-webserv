
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

void Response::HandleGet(Request &req, Location &loc)
{
    std::map<std::string, std::string> mimeTypes = mime_types_init();
    std::string contentType = getContentType(req.getFile(), mimeTypes);

    _resp.first = "HTTP/1.1 200 OK\nContent-Type: ";
    _resp.first += contentType;
    std::ifstream file;
    file.open(req.getFile(), std::ios::binary | std::ios::ate);
    if (!file.is_open() || access(req.getPath().c_str(), R_OK) == -1)
    {
        //check if the file exist
        if (access(req.getFile().c_str(), F_OK) == -1)
        {    
            _resp.first = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 13\n\n404 not found";
            _resp.second = 84;
            return ;
        }
        std::cout << "dir ="<< req.getPath() << std::endl;
        //check if the file has read permission
        if (access(req.getFile().c_str(), R_OK) == -1 || access(req.getPath().c_str(), R_OK) == -1)
        {
            _resp.first = "HTTP/1.1 403 Forbidden\nContent-Type: text/html\nContent-Length: 15\n\n403 forbidden";
            _resp.second = 84;
            return ;
        }
    }
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
}

void Response::HandleDelete(Request &req, Location &loc)
{
}


// std::string    Response::auto_indexing(const char *dir)
// {
//     std::string resp = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length:";
//     std::string str = "<html><head><title>Index of /</title></head><body><h1>Index of /</h1><hr><pre>";
//     DIR *dp;
//     struct dirent *dirp;
//     if((dp  = opendir(dir)) == NULL)
//     {
//         resp = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 13\n\n404 not found";
//         return resp;
//     }
//     while ((dirp = readdir(dp)) != NULL)
//     {
//         str += "<a href=\"";
//         str += dirp->d_name;
//         str += "\">";
//         str += dirp->d_name;
//         str += "</a>\n";
//     }
//     str += "</pre><hr></body></html>";
//     resp += std::to_string(str.length());
//     resp += "\n\n";
//     resp += str;
//     return resp;
// }

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
    return "application/octet-stream";
}

std::pair<std::string, u_long> prepare_response(Request & req, Server & server)
{
    Response respp(req, server);

    return respp.getResp();
}