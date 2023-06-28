#pragma once

#include "request.hpp"
// #include "response.hpp"



class Cgi
{
    private :
    std::pair<std::string, std::string> _cgi;
    std::map<std::string, std::string> env;
    char **_env;
    // cgi::Cgi _cgi;

    public :
    Cgi();
    ~Cgi();
    std::pair<std::string, std::string> & get_Cgi();
    void set_Cgi(std::pair<std::string, std::string> const &cgi);
    void initEnv(Request const & req, std::string const & server_name, std::string const & root);
    char **getEnv();
    // void cgi_exec(Request &req, Response &resp, Server &server);
    void print_cgi();
    void clear();
    int  OK(){
        if (this->_cgi.first.empty() || this->_cgi.second.empty())
            return 0;
        return 1;
    };
};