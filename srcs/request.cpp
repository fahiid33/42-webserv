
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
    this->headers.clear();
    this->timeOut = 30;
    this->keepAlive = true;
    this->started = 0;
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
    this->headers.clear();
    this->keepAlive = true;
    this->body = "";
    this->conn = "";
    this->started = 0;
    this->timeOut = 30;
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
    this->headers = req.headers;
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
    this->headers = req.headers;
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

void searchThrow(std::string &out, std::string &line, std::string const &search)
{
    size_t pos = line.find(search);
    if (pos != std::string::npos)
    {
        out = line.substr(0, pos);
        line = line.substr(pos + search.length(), line.length() - 1);
    }
    else
        throw std::invalid_argument("0");
}

void Request::parseFirstLine(std::string &line)
{
    std::string str = this->request;
    searchThrow(method, line, " ");
    if (method != "GET" && method != "POST" && method != "DELETE")
    {
        std::cout << "method: " << method << std::endl;
        if (method == "PUT")
            throw std::invalid_argument("10");
        throw std::invalid_argument("8");
    }
    searchThrow(path, line, " ");
    if (path.length() > 2048)
        throw std::invalid_argument("0");
    if (path.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%") != std::string::npos)
        throw std::invalid_argument("0");
    searchThrow(version, line, "\r");
    if (version != "HTTP/1.1")
        throw std::invalid_argument("0");
    this->file = path.substr(path.find_last_of('/') + 1, path.length() - 1);
    this->path = path.substr(0, path.find_last_of('/') + 1);
}

void Request::ParseHeaders(std::istringstream &file)
{
    std::string line;
    std::string key;
    std::string value;
    
    while(std::getline(file, line) && line != "\r")
    {
        searchThrow(key, line, ":");
        searchThrow(value, line, "\r");
                
        if (key == "Host:" && value.empty()) // check against the server name
            throw std::invalid_argument("1");
        else if (key == "Connection:")
        {
            if (value == "keep-alive")
                keepAlive = true;
            else if (value == "close")
                keepAlive = false;
            else
                throw std::invalid_argument("0");
        }
        else if (key == "Content-Length:" && (value.empty() || value.find_first_not_of("0123456789") == std::string::npos)) // check the value against the server max_body size
        {
            throw std::invalid_argument("0");
        }
        else if (key == "Transfer-Encoding:")
        {
            if (value.empty())
                throw std::invalid_argument("0");
            else if (key != "chunked")
                throw std::invalid_argument("5");
        }
        else if (key == "Keep-Alive:")
        {
            std::string chck;
            searchThrow(chck, value, "=");
            value = value.substr(0, value.length() - 2);
            if (chck != "timeout"){
                throw std::invalid_argument("0");
            }
            else if (value.find_first_not_of("0123456789") != std::string::npos)
                throw std::invalid_argument("0");
            else
            {
                keepAlive = true;
                timeOut = atoi(value.c_str());
            }
        }
        this->headers[key] = value;
    }
    
    if (method == "POST" && ((this->headers.find("Content-Length:") == this->headers.end() &&
    this->headers.find("Transfer-Encoding:") == this->headers.end()) || (this->headers.find("Content-Length:") !=
    this->headers.end() && this->headers.find("Transfer-Encoding:") != this->headers.end())))
            throw std::invalid_argument("9");
    if ((method == "GET" || method == "HEAD") && (this->headers.find("Content-Length:") !=
    this->headers.end() || this->headers.find("Transfer-Encoding:") != this->headers.end()))
        throw std::invalid_argument("9");
    if (keepAlive)
        started = time(NULL);
}

Request::Request(const char* request)
{
    std::string line;
    std::istringstream file;

    this->clear();
    this->request = request;
    file.str(request);
    std::getline(file, line);
    this->parseFirstLine(line);
    this->ParseHeaders(file);    
    std::string body = this->request.substr(this->request.find("\r\n\r\n") + 4, this->request.length() - 1);
    this->body = body;
    if ((method == "GET" || method == "HEAD") && !body.empty())
        throw std::invalid_argument("9");
}

Request::~Request()
{
    this->clear();
}

std::map<std::string, std::string> Request::getHeaders()
{
    return this->headers;
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

void Request::setStarted(time_t started)
{
    this->started = started;
}

void Request::setRequest(std::string request) 
{
    this->request = request;
}

int Request::getContent_length()
{
    return this->content_length;
}

void Request::setBody(std::string body)
{
    this->body = body;
}
