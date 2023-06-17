
#include "../includes/request.hpp"

Request::Request()
{
    this->request = "";
}



Request::Request(std::string request)
{
    std::istringstream iss;
    std::istringstream file;
    std::string line;

    this->request = request;
    content_length = -1;
    tr_enc = "";
    method = "";
    path = "";
    version = "";
    file.str(request);
    std::getline(file, line);
    iss.str(line);
    iss >> method >> path >> version;
    if(iss >> version)
        throw std::invalid_argument("Invalid request");
    if (path.size() > 2048)
        throw std::invalid_argument("Invalid request");
    if (path.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%") != std::string::npos)
        throw std::invalid_argument("Invalid request");
    this->file = path.substr(path.find_last_of('/') + 1, path.length() - 1);
    this->path = path.substr(0, path.find_last_of('/') + 1);
    std::cout << request << std::endl;
    while(std::getline(file, line))
    {
        iss.clear();
        iss.str(line);
        if (iss >> line)
        {
            if (line == "Host:")
            {
                iss >> host;
                if (iss >> host)
                    throw std::invalid_argument("Invalid request");
            }
            if (line == "Connection:")
            {
                iss >> conn;
                if (iss >> conn)
                    throw std::invalid_argument("Invalid request");
            }
            if (line == "Content-Length:")
            {
                iss >> content_length;
                if (iss >> content_length)
                    throw std::invalid_argument("Invalid request");
            }
            if (line == "Transfer-Encoding:")
            {
                iss >> tr_enc;
                if (iss >> tr_enc)
                    throw std::invalid_argument("Invalid request");
                else if (tr_enc != "chunked")
                    throw std::invalid_argument("Invalid request");
            }
        }
    }
    if ((method != "GET" && method != "POST" && method != "DELETE") || version != "HTTP/1.1")
        throw std::invalid_argument("Invalid request");
    if (method == "POST" && content_length == -1 && tr_enc == "")
            throw std::invalid_argument("Invalid request");
    if (strlen(std::strstr(request.c_str(), "\r\n\r\n") ? std::strstr(request.c_str(), "\r\n\r\n")  : "a") - 4 > 100 )
    {
            throw std::invalid_argument("Invalid request");
    }
    
    
}

Request::~Request()
{
}

std::string Request::getPath()
{
    return this->path;
}

std::string Request::getMethod()
{
    return this->method;
}

std::string Request::getVersion()
{
    return this->version;
}

std::string Request::getHost()
{
    return this->host;
}

std::string Request::getFile()
{
    return this->file;
}

std::string Request::getConn()
{
    return this->conn;
}

std::string Request::getTr_enc()
{
    return this->tr_enc;
}

std::string Request::getRequest()
{
    return this->request;
}

void Request::setRequest(std::string request)
{
    this->request = request;
}

int Request::getContent_length()
{
    return this->content_length;
}