#include "../includes/request.hpp"
#include "../includes/socket.hpp"
#include "../includes/server.hpp"

const char* YELLOW1 = "\033[33m";

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
    this->started = time(NULL);
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
    this->started = time(NULL);
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
    if (pos != std::string::npos) {
        out = line.substr(0, pos);
        line = line.substr(pos + search.length(), line.length() - 1);
    } else
        throw std::invalid_argument("400");
}

void Request::parseFirstLine(std::string &line)
{
    std::string str = this->request;
    searchThrow(method, line, " ");
    if (method != "GET" && method != "POST" && method != "DELETE") {
        throw std::invalid_argument("405");
    }
    searchThrow(path, line, " ");
    if (path.length() > 2083)
        throw std::invalid_argument("414");
    if (path.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=") != std::string::npos)
        throw std::invalid_argument("400");
    searchThrow(version, line, "\r");
    if (version != "HTTP/1.1")
        throw std::invalid_argument("505");
    if (path.find_last_of('?') != std::string::npos)
        this->query = path.substr(path.find('?') + 1, path.length() - 1);
    if (path.find_last_of('/') != std::string::npos)
        this->file = path.substr(path.find_last_of('/') + 1, path.find_first_of('?') - path.find_last_of('/') - 1) ;
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
        searchThrow(key, line, ": ");
        searchThrow(value, line, "\r");
        if (key == "Host") {
            host = value.substr(0, value.find(':'));
        } else if (key == "Connection") {
            if (value == "keep-alive")
                keepAlive = true;
            else if (value == "close")
                keepAlive = false;
            else
                throw std::invalid_argument("400");
        } else if (key == "Content-Length") {
            if (value.empty())
                throw std::invalid_argument("411");
            if (value.find_first_not_of("0123456789") != std::string::npos)
                throw std::invalid_argument("400");
        } else if (key == "Transfer-Encoding") {
            if (value.empty())
                throw std::invalid_argument("400");
            else if (value != "chunked") {
                throw std::invalid_argument("501");
            }
        } else if (key == "Content-Type") {
            if (value.empty())
                throw std::invalid_argument("400");
            else if (value.find("multipart/form-data") == 0) {
                std::string chck;
                std::string valos = value;
                searchThrow(line, value, "; ");
                searchThrow(chck, value, "=");
                if (chck != "boundary")
                    throw std::invalid_argument("400");
                value = valos;
            }
        }
        else if (key == "Referer"){

            value = value.substr(value.find_first_of("//") + 2, value.length() - 1);
            value = value.substr(value.find_first_of("/"), value.length() - 1);
            if (value.find_last_of('/') != std::string::npos)
                value = value.substr(0, value.find_last_of('/') + 1);
        }
        else if (key == "Keep-Alive") {
            std::string chck;
            searchThrow(chck, value, "=");
            searchThrow(value, value, ", ");
            if (chck != "timeout") {
                throw std::invalid_argument("400");
            } else if (value.find_first_not_of("0123456789") != std::string::npos)
                throw std::invalid_argument("400");
            else {
                timeOut = stoi(value);
            }
        }
        this->headers.insert(std::make_pair(key, value));
    }
    std::map<std::string, std::string>::iterator it = this->headers.find("Host");
    if (it == this->headers.end())
        throw std::invalid_argument("400");
    if (method == "POST" && (this->headers.find("Content-Length") == this->headers.end() && this->headers.find("Transfer-Encoding") == this->headers.end()))
            throw std::invalid_argument("411");
    else if (method == "POST" && (this->headers.find("Content-Length") != this->headers.end() && this->headers.find("Transfer-Encoding") != this->headers.end()))
        throw std::invalid_argument("400");
    if ((method == "GET" || method == "HEAD") && (this->headers.find("Content-Length") !=
    this->headers.end() || this->headers.find("Transfer-Encoding") != this->headers.end()))
        throw std::invalid_argument("400");
}

Request::Request(std::pair <Socket, Server> & client, std::vector<Server>& servers)
{
    std::string line;
    std::istringstream file;
    std::vector<unsigned char> pattern;
    std::vector<unsigned char> request;

    pattern.push_back('\r');
    pattern.push_back('\n');
    pattern.push_back('\r');
    pattern.push_back('\n');

    request = client.first.getrequest();
    std::cout << "REQUEST: " << std::endl;
    this->clear();
    std::vector<unsigned char>::iterator pos = std::search(request.begin(), request.end(), pattern.begin(), pattern.end());
    std::string str(request.begin(), pos + 2);

    this->request = str;
    std::cout << str << std::endl;
    file.str(str);
    std::getline(file, line);
    this->parseFirstLine(line);
    this->ParseHeaders(file);
    std::vector<Server>::iterator it = servers.begin();
    while (it != servers.end())
    {
        if (std::find(it->getServerNames().begin(), it->getServerNames().end(), host) != it->getServerNames().end())
            break;
        it++;
    }
    if (it != servers.end())
        client.second = *it;
    else
        client.second = servers[0];
    if (pos + 4 != request.end()) {    std::vector<unsigned char> body(pos + 4, request.end());
        this->body = body;
    }
    if ((method == "GET" || method == "HEAD") && !body.empty())
        throw std::invalid_argument("400");
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

void Request::setFile(std::string file)
{
    this->file = file;
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

std::vector<unsigned char> const & Request::getBody() const
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

void Request::setBody(const std::vector<unsigned char> & body)
{
    this->body = body;
}


void Request::parseChunkedBody(std::vector<unsigned char> const & request) {

    std::vector<unsigned char> pattern;
    std::vector<unsigned char> newBody;
    pattern.clear();
    newBody.clear();
    pattern.push_back('\r');
    pattern.push_back('\n');

    std::vector<unsigned char>::const_iterator pos = request.begin();
    std::vector<unsigned char>::const_iterator end = request.end();

    std::stringstream ss;
    int chunkSize = 0;
    while (pos != request.end() - 5) {
        end = std::search(pos, request.end(), pattern.begin(), pattern.end());
        ss << std::hex << std::string(pos, end);
        ss >> chunkSize;
        ss.clear();
        ss.str(std::string());
        pos = end + 2;
        end = pos + chunkSize;
        newBody.insert(newBody.end(), pos, end);
        pos = end + 2;
    }
    this->body = newBody;
}
