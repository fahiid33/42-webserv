
#include "../includes/request.hpp"

Request::Request()
{
    this->request = "";
    this->content_length = -1;
    this->tr_enc = "";
    this->method = "";
    this->path = "";
    this->query = "";
    this->boundary = "";
    this->version = "";
    this->host = "";
    this->file = "";
    this->body.clear();
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
    this->query = "";
    this->method = "";
    this->boundary = "";
    this->path = "";
    this->version = "";
    this->host = "";
    this->file = "";
    this->headers.clear();
    this->keepAlive = true;
    this->body.clear();
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
    this->query = req.query;
    this->boundary = req.boundary;
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
    this->boundary = req.boundary;
    this->query = req.query;
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
        throw std::invalid_argument("2");
    if (path.find_last_of('/') != std::string::npos)
        this->file = path.substr(path.find_last_of('/') + 1, path.length() - 1);
    if (path.find_last_of('?') != std::string::npos)
        this->query = path.substr(path.find('?') + 1, path.length() - 1);
    if (path.find_last_of('/') != std::string::npos)
        this->path = path.substr(0, path.find_last_of('/') + 1);
}

void Request::ParseHeaders(std::istringstream &file)
{
    std::string line;
    std::string key;
    std::string value;
    
    while(std::getline(file, line) && line != "\r")
    {
        key.clear();
        value.clear();
        std::cout << "line: " << line << std::endl;
        searchThrow(key, line, ": ");
        searchThrow(value, line, "\r");
        // hadi katfaili f aker line content-type dyal postman
        if (key == "Host" && value.empty()) // check against the server name
            throw std::invalid_argument("0");
        else if (key == "Connection")
        {
            if (value == "keep-alive")
                keepAlive = true;
            else if (value == "close")
                keepAlive = false;
            else
                throw std::invalid_argument("0");
        }
        else if (key == "Content-Length" && (value.empty() || value.find_first_not_of("0123456789") != std::string::npos)) // check the value against the server max_body size
        {
            std::cout << "value: " << value << std::endl;
            std::cout << "key: " << key << std::endl;
            throw std::invalid_argument("0");
        }
        else if (key == "Transfer-Encoding")
        {
            if (value.empty())
                throw std::invalid_argument("0");
            else if (key != "chunked")
                throw std::invalid_argument("5");
        }
        else if (key == "Content-Type" && value.find("multipart/form-data") == 0)
        {
            std::string chck;
            std::string valos = value;
            searchThrow(line, value, "; ");
            searchThrow(chck, value, "=");
            if (chck != "boundary")
                throw std::invalid_argument("0");
            value = valos;
        }
        else if (key == "Keep-Alive")
        {
            std::string chck;
            searchThrow(chck, value, "=");
            searchThrow(value, value, ", ");
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
        this->headers.insert(std::make_pair(key, value));
    }
    
    if (method == "POST" && (this->headers.find("Content-Length") == this->headers.end() && this->headers.find("Transfer-Encoding") == this->headers.end()))
            throw std::invalid_argument("9");
    else if (method == "POST" && (this->headers.find("Content-Length") != this->headers.end() && this->headers.find("Transfer-Encoding") != this->headers.end()))
        throw std::invalid_argument("0");
    if ((method == "GET" || method == "HEAD") && (this->headers.find("Content-Length") !=
    this->headers.end() || this->headers.find("Transfer-Encoding") != this->headers.end()))
        throw std::invalid_argument("9");
}

Request::Request(std::vector<unsigned char> request)
{
    std::string line;
    std::istringstream file;
    std::vector<unsigned char> pattern;
    pattern.push_back('\r');
    pattern.push_back('\n');
    pattern.push_back('\r');
    pattern.push_back('\n');

    this->clear();
    auto pos = std::search(request.begin(), request.end(), pattern.begin(), pattern.end());
    std::string str(request.begin(), pos + 2);

    this->request = str;
    //// this is header
    std::cout << "request: " << str << std::endl;
    file.str(str);
    std::getline(file, line);
    this->parseFirstLine(line);
    this->ParseHeaders(file);
    //// end of header
    //// this is body
    if (pos + 4 != request.end())
    {    std::vector<unsigned char> body(pos + 4, request.end());
        this->body = body;}
    //// end of body
    if ((method == "GET" || method == "HEAD") && !body.empty())
        throw std::invalid_argument("9");
    started = time(NULL);
}

Request::~Request()
{
    this->clear();
}

std::map<std::string, std::string> const & Request::getHeaders() const
{
    return this->headers;
}

std::string Request::getPath() const
{
    return this->path;
}

std::string Request::getMethod() const
{
    return this->method;
}

std::string Request::getQuery() const
{
    return this->query;
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


std::string Request::getFile() const
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

std::vector<unsigned char> & Request::getBody()
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

void Request::setBody(std::vector<unsigned char>  body)
{
    this->body = body;
}
