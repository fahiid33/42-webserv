#include "../includes/cgi.hpp"

Cgi::Cgi() // should be Cgi(server)
{
}

Cgi::~Cgi()
{
}

void Cgi::initEnv(Request const & req, std::string const & server_name)
{
    this->env["HTTP_USER_AGENT"] = ""; // kayna fl header
    this->env["SCRIPT_FILENAME"] = ""; // The full path to the CGI script.
    this->env["CONTENT_LENGTH"] = ""; // length dl body d request
    this->env["CONTENT_TYPE"] = ""; // application/x-www-form-urlencoded or multipart/form-data, etc.
    this->env["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->env["PATH_TRANSLATED"] = ""; // root + path (requested file)
    this->env["QUERY_STRING"] = ""; // url mor ? 
    this->env["REQUEST_METHOD"] = ""; // GET, POST, HEAD
    this->env["SCRIPT_NAME"] = ""; // The name of the CGI script.
    this->env["SERVER_NAME"] = ""; // kayna fl config, normally server object
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