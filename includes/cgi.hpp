#pragma once

#include "request.hpp"
#include "response.hpp"



class Cgi
{
    private :
    std::pair<std::string, std::string> _cgi;
    std::map<std::string, std::string> env;
    char **_env;
    // cgi::Cgi _cgi;

    public :
    Cgi();
    // Cgi(Cgi const & cgi);
    ~Cgi();
    Cgi(std::string path, std::string ext);
    std::pair<std::string, std::string> const &  get_Cgi() const;
    void set_Cgi(std::pair<std::string, std::string> const &cgi);
    void initEnv(Request const & req, std::string const & server_name, std::string const & root);
    char **getEnv();
    // void cgi_exec(Request &req, Response &resp, Server &server);
    void print_cgi();
    void clear();
    void runCgi(Request &req, Location &loc, Response &resp);
    int  OK(){
        if (this->_cgi.first.empty() || this->_cgi.second.empty())
                  return 0;
        return 1;
    };
};