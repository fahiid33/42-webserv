#pragma once

#include "server.hpp"
#include "request.hpp"
#include "response.hpp"



class cgi
{
    public :
    typedef std::vector<std::pair <std::string, std::string > > Cgi;

    private :
    cgi::Cgi _cgi;

    public :
    cgi();
    ~cgi();
    cgi::Cgi & getCgi();
    void setCgi(cgi::Cgi &cgi);
    void cgi_exec(Request &req, Response &resp, Server &server);
    void print_cgi();
};