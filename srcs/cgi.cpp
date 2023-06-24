#include "../includes/cgi.hpp"

cgi::cgi() // should be cgi(server)
{
    Server server;
    std::vector <std::string> cgi_path = server.get_cgi(); // should be sent by methods
    
    // first = extension, second = path
    for (int i = 0; i < cgi_path.size(); i++)
    {
        std::string extension = cgi_path[i].substr(cgi_path[i].find_last_of(".") + 1);
        std::string path = cgi_path[i].substr(0, cgi_path[i].find_last_of("."));
        _cgi.push_back(std::make_pair(extension, path));
    }
    print_cgi();
}

cgi::~cgi()
{
}

void cgi::print_cgi()
{
    for (int i = 0; i < _cgi.size(); i++)
    {
        std::cout << _cgi[i].first << " " << _cgi[i].second << std::endl;
    }
}

cgi::Cgi & cgi::getCgi()
{
    return _cgi;
}

void cgi::setCgi(cgi::Cgi &cgi)
{
    _cgi = cgi;
}

void cgi::cgi_exec(Request &req, Response &resp, Server &server)
{

}
