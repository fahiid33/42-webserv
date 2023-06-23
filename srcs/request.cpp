
#include "../includes/request.hpp"

Request::Request()
{
    this->request = "";
    this->content_length = -1;
    this->tr_enc = "";
    this->method = "";
    this->path = "";
    this->version = "";
    this->host = "";
    this->file = "";
    this->body = "";
    this->conn = "";
    this->timeOut = 8;
    this->keepAlive = true;
    this->started = time(NULL);
}

void Request::clear()
{
    this->request = "";
    this->content_length = -1;
    this->tr_enc = "";
    this->method = "";
    this->path = "";
    this->version = "";
    this->host = "";
    this->file = "";
    this->keepAlive = true;
    this->body = "";
    this->conn = "";
    this->started = time(NULL);
    this->timeOut = 8;
}

Request::Request(const Request &req)
{
    this->request = req.request;
    this->content_length = req.content_length;
    this->tr_enc = req.tr_enc;
    this->method = req.method;
    this->path = req.path;
    this->body = req.body;
    this->version = req.version;
    this->host = req.host;
    this->keepAlive = req.keepAlive;
    this->timeOut = req.timeOut;
    this->file = req.file;
    this->conn = req.conn;
    this->started = req.started;
}

Request &Request::operator=(const Request &req)
{
    this->request = req.request;
    this->content_length = req.content_length;
    this->tr_enc = req.tr_enc;
    this->body = req.body;
    this->method = req.method;
    this->path = req.path;
    this->timeOut = req.timeOut;
    this->version = req.version;
    this->host = req.host;
    this->file = req.file;
    this->keepAlive = req.keepAlive;
    this->conn = req.conn;
    this->started = req.started;
    return *this;
}

Request::Request(const char* request)
{
    std::istringstream iss;
    std::istringstream file;
    std::string line;

    this->clear();
    this->request += request;
    // std::cout << request << std::endl;
    file.str(request);
    std::getline(file, line);
    iss.str(line);
    iss >> method >> path >> version;
    if(iss >> version)
        throw std::invalid_argument("0");
    if (path.size() > 2048)
        throw std::invalid_argument("0");
    if (path.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%") != std::string::npos)
        throw std::invalid_argument("0");
    this->file = path.substr(path.find_last_of('/') + 1, path.length() - 1);
    this->path = path.substr(0, path.find_last_of('/') + 1);
    // std::cout << request << std::endl;
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
                    throw std::invalid_argument("1");
            }
            if (line == "Connection:")
            {
                iss >> conn;
                if (iss >> conn)
                    throw std::invalid_argument("0");
                else if (conn == "keep-alive")
                    keepAlive = true;
            }
            if (line == "Content-Length:")
            {
                iss >> content_length;
                if (iss >> content_length)
                    throw std::invalid_argument("0");
            }
            if (line == "Transfer-Encoding:")
            {
                iss >> tr_enc;
                if (iss >> tr_enc)
                    throw std::invalid_argument("0");
                else if (tr_enc != "chunked")
                    throw std::invalid_argument("5");
            }
            if (line == "Keep-Alive:")
            {
                // std::string chk("");
                std::getline(iss, line , '=');
                if (line != "timeout"){
                    throw std::invalid_argument("0");
                }
                else if(!(iss >> timeOut) || timeOut < 0){
                    throw std::invalid_argument("0");
                }
            }
        }
    }
    if ((method != "GET" && method != "POST" && method != "Post" && method != "DELETE") || version != "HTTP/1.1")
    {
        std::cout << "method: " << method << std::endl;
        throw std::invalid_argument("8");
    }
    if (method == "POST" && content_length == -1 && tr_enc == "")
            throw std::invalid_argument("9");
    if (keepAlive && timeOut == -1)
    {
        started = time(NULL);
    }
    // std::string body = request.substr(request.find("\r\n\r\n") + 4, request.length() - 1);
    // i(body.c_str()) > 100 )
    // {
    //         throw std::invalid_argument("Invalid request");
    // }
    // bool flag = (*(request.end() - 1) == '\n' && *(request.end() - 2) == '\r' && *(request.end() - 3) == '\n' && *(request.end() - 4) == '\r');
    // if (!flag)
    //     throw std::invalid_argument("Invalid request");
    // else
    //     this->body = body;
}

Request::~Request()
{
    this->clear();
}

std::string Request::getPath()
{
    return this->path;
}

std::string Request::getMethod()
{
    return this->method;
}

time_t Request::getStarted()
{
    return this->started;
}

std::string Request::getVersion()
{
    return this->version;
}

std::string Request::getHost()
{
    return this->host;
}

time_t Request::getTimeOut()
{
    return this->timeOut;
}


std::string Request::getFile()
{
    return this->file;
}

bool Request::getConn()
{
    return this->keepAlive;
}

std::string Request::getTr_enc()
{
    return this->tr_enc;
}

std::string Request::getBody()
{
    return this->body;
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