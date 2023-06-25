#include "../includes/cgi.hpp"

Cgi::Cgi() // should be Cgi(server)
{
}

Cgi::~Cgi()
{
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