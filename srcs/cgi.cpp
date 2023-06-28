#include "../includes/cgi.hpp"

Cgi::Cgi() // should be Cgi(server)
{
}

Cgi::~Cgi()
{
}

void Cgi::initEnv(Request const & req, std::string const & server_name, std::string const & root)
{

//     FCGI_ROLE: RESPONDER
//      SCRIPT_FILENAME: /usr/lib/cgi-bin/cgi_python.py
//    GATEWAY_INTERFACE: CGI/1.1
//      SERVER_SOFTWARE: nginx
//         QUERY_STRING: 
//       REQUEST_METHOD: POST
//         CONTENT_TYPE: multipart/form-data
//       CONTENT_LENGTH: 18
//          SCRIPT_NAME: /cgi-bin/cgi_python.py
//          REQUEST_URI: /cgi-bin/cgi_python.py
//         DOCUMENT_URI: /cgi-bin/cgi_python.py
//        DOCUMENT_ROOT: /usr/lib
//      SERVER_PROTOCOL: HTTP/1.1
//          REMOTE_ADDR: 127.0.0.1
//          REMOTE_PORT: 44688
//          SERVER_ADDR: 127.0.0.1
//          SERVER_PORT: 999
//          SERVER_NAME: localhost
//            HTTP_HOST: localhost:999
//      HTTP_USER_AGENT: curl/7.88.1
//          HTTP_ACCEPT: */*
    if (req.getHeaders().find("Content-Length") != req.getHeaders().end())
        this->env["CONTENT_LENGTH"] = req.getHeaders().find("Content-Length")->second;
    if (req.getHeaders().find("User-Agent") != req.getHeaders().end())
        this->env["HTTP_USER_AGENT"] = req.getHeaders().find("User-Agent")->second;
    if (req.getHeaders().find("Content-Type") != req.getHeaders().end())
        this->env["CONTENT_TYPE"] = req.getHeaders().find("Content-Type")->second;; // application/x-www-form-urlencoded or multipart/form-data, etc.
    this->env["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->env["REQUEST_METHOD"] = req.getMethod(); // GET, POST, HEAD
    this->env["SERVER_NAME"] = server_name; // kayna fl config, normally server object
    this->env["SCRIPT_NAME"] = req.getPath() + req.getFile(); // The name of the CGI script.
    this->env["REQUEST_URI"] = req.getPath() + req.getFile(); // The name of the CGI script.
    this->env["DOCUMENT_URI"] = req.getPath() + req.getFile(); // The name of the CGI script.
    this->env["DOCUMENT_ROOT"] = root; // The name of the CGI script.
    this->env["QUERY_STRING"] = req.getQuery(); // url mor ? 
    this->env["SCRIPT_FILENAME"] = root + req.getPath() + req.getFile(); // The full path to the CGI script.
    this->env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->env["SERVER_SOFTWARE"] = "webserv";
}

void Cgi::print_cgi()
{
    std::cout << "CGI: " << this->_cgi.first << " " << this->_cgi.second << std::endl;
}

std::pair<std::string, std::string> & Cgi::get_Cgi()
{
    return this->_cgi;
}

char **Cgi::getEnv()
{
    int i = 0;
    this->_env = new char*[this->env.size() + 1];
    for (std::map<std::string, std::string>::iterator it = this->env.begin(); it != this->env.end(); it++)
    {
        this->_env[i] = new char[it->first.length() + it->second.length() + 2];
        strcpy(this->_env[i], (it->first + "=" + it->second).c_str());
        i++;
    }
    this->_env[i] = NULL;
    return this->_env;
}

void Cgi::set_Cgi(std::pair<std::string, std::string> const &cgi)
{
    this->_cgi = cgi;
}

// void Cgi::cgi_exec(Request &req, Response &resp, Server &server)
// {

// }

void Cgi::clear()
{
    this->_cgi.first.clear();
    this->_cgi.second.clear();
}