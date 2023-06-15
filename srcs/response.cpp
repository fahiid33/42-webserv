#include "../includes/response.hpp"

Response::Response() : _offset(0) {}

Response::~Response() {}

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

std::string    Response::auto_indexing(const char *dir)
{
    std::string resp = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length:";
    std::string str = "<html><head><title>Index of /</title></head><body><h1>Index of /</h1><hr><pre>";
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir)) == NULL) 
    {
        resp = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 13\n\n404 not found";
        return resp;
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
    return resp;
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
    return "application/octet-stream";
}

std::pair<std::string, u_long> prepare_response(const char *file_name,const char *dir)
{
    Response respp;
    std::pair<std::string, u_long> resp ;
    std::time_t currentTime = std::time(nullptr);
    std::time_t expirationTime = currentTime + 666666666666;
    std::map<std::string, std::string> mimeTypes = respp.mime_types_init();
    std::string contentType = respp.getContentType(file_name, mimeTypes);


    std::cout << "contentType = " << contentType << std::endl;
    std::cout << "file = " << file_name << std::endl;
    resp.first = "HTTP/1.1 200 OK\nExpires: ";
    std::ostringstream timeStream;
    timeStream << std::put_time(std::gmtime(&expirationTime), "%a, %d %b %Y %H:%M:%S GMT");
    resp.first += timeStream.str();

    resp.first += "\r\n";
    resp.first += "Content-Type: ";
    resp.first += contentType;
    std::ifstream file;
    file.open(file_name, std::ios::binary | std::ios::ate);
    if (!file.is_open() || access(dir, R_OK) == -1)
    {
        //check if the file exist
        if (access(file_name, F_OK) == -1)
        {    
            resp.first = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 13\n\n404 not found";
            resp.second = 13;
            return resp;
        }
        std::cout << "dir ="<< dir << std::endl;
        //check if the file has read permission
        if (access(file_name, R_OK) == -1 || access(dir, R_OK) == -1)
        {
            resp.first = "HTTP/1.1 403 Forbidden\nContent-Type: text/html\nContent-Length: 15\n\n403 forbidden";
            resp.second = 15;
            return resp;
        }
    }
    std::string str;
    resp.first += "\nContent-Length:";
    resp.second = file.tellg();
    file.seekg(0, std::ios::beg);
    resp.first += std::to_string(resp.second);
    resp.first += "\n\n";
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    resp.first += content;
    return resp;
}