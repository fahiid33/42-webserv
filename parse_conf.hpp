#ifndef PARSE_CONF_HPP
#define PARSE_CONF_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>



class Location
{
    private:
        std::string                                         _locationNumber;
        std::string                                         _locationPath;
        std::vector<std::string>                            _allowedMethods;
        std::string                                         _root;
        std::vector<std::string>                            _index;
        bool                                                _autoIndex;
        std::string                                         _uploadPath;
        std::pair<std::string, std::string>                 _redirection;
        std::vector<std::string>                            _cgi_extension;     // .php
        std::vector<std::string>                            _cgi_path;          // /usr/bin/php-cgi

    public:
        Location();
        ~Location();

        std::string                                         & getLocationNumber();
        std::string                                         & getLocationPath();
        std::vector<std::string>                            & getAllowedMethods();
        std::string                                         & getRoot();
        std::vector<std::string>                            & get_cgi_extension();
        std::vector<std::string>                            & get_cgi_path();

        std::vector<std::string>                            & getIndex();
        bool                                                & getAutoIndex();
        std::string                                         & getUploadPath();
        std::pair<std::string, std::string>                 & getRedirection();

        void                                                setLocationNumber(std::string locationNumber);
        void                                                setLocationPath(std::string locationPath);
        void                                                setAllowedMethods(std::vector<std::string> allowedMethods);
        void                                                setRoot(std::string root);
        void                                                setIndex(std::vector<std::string> index);
        void                                                setAutoIndex(bool autoIndex);
        void                                                setUploadPath(std::string uploadPath);
        void                                                setRedirection(std::pair<std::string, std::string> redirection);
        void                                                print_location(){
            std::cout << "locationNumber : " << _locationNumber << std::endl;
            std::cout << "locationPath : " << _locationPath << std::endl;
            std::cout << "allowedMethods : ";
            for (size_t i = 0; i < _allowedMethods.size(); i++)
                std::cout << _allowedMethods[i] << " ";
            std::cout << std::endl;
            std::cout << "root : " << _root << std::endl;
            std::cout << "index : ";
            for (size_t i = 0; i < _index.size(); i++)
                std::cout << _index[i] << " ";
            std::cout << std::endl;
            std::cout << "autoIndex : " << _autoIndex << std::endl;
            std::cout << "uploadPath : " << _uploadPath << std::endl << "cgi_extension : ";
            for (size_t i = 0; i < _cgi_extension.size(); i++)
                std::cout  << _cgi_extension[i] << " ";
            std::cout << std::endl << "cgi_path : ";
            for (size_t i = 0; i < _cgi_path.size(); i++)
                std::cout << _cgi_path[i] << " "; 
            std::cout << std::endl;
            std::cout << "redirection : " << _redirection.first << " " << _redirection.second << std::endl;
        }
};

class Socket{
    private:
        int _socket_fd;
        struct sockaddr_in  *_sockaddr;
    public:
        Socket();
        ~Socket();
        int getSocket_fd();
        struct sockaddr_in  *getSockaddr();
        void setSocket_fd(int socket_fd);
        void setSockaddr(struct sockaddr_in  *sockaddr);
};

class Server{
    private:
        Socket _socket;
        size_t _port;
        std::string _host;
        std::vector<std::string> _serverNames;
        std::vector<Location> _locations;
        std::vector<std::pair<size_t, std::string> > _error_pages;
        size_t _clientMaxBodySize;
        std::vector<std::string> _mime_types;
    public:
        Server();
        ~Server();
        Socket & getSocket();
        size_t & getPort();
        std::vector<std::string> & getServerNames();
        std::vector<Location> & getLocations();
        size_t & getClientMaxBodySize();
        std::string & getHost();
        std::vector<std::pair<size_t, std::string> > & getError_pages();
        std::vector<std::string> & getMime_types();

        void setSocket(Socket socket);
        void setPort(size_t port);
        void setServerNames(std::vector<std::string> serverNames);
        void setLocations(std::vector<Location> locations);
        void setHost(std::string host);
        void setClientMaxBodySize(size_t clientMaxBodySize);
        void setError_pages(std::vector<std::pair<size_t, std::string> > error_pages);
        void setMime_types(std::vector<std::string> mime_types);
        void print_server(){
            std::cout << "port : " << _port << std::endl;
            std::cout << "host : " << _host << std::endl;
            std::cout << "clientMaxBodySize : " << _clientMaxBodySize << std::endl;
            std::cout << "serverNames : ";
            for (size_t i = 0; i < _serverNames.size(); i++)
                std::cout << _serverNames[i] << " ";
            std::cout << std::endl;
            std::cout << "locations : " << std::endl;
            for (size_t i = 0; i < _locations.size(); i++)
            {
                std::cout << "------------------------++" "location " << i << " : ------------------------++"  << std::endl;
                _locations[i].print_location();
            }
            std::cout << std::endl;
            std::cout << "error_pages : ";
            for (size_t i = 0; i < _error_pages.size(); i++)
                std::cout << _error_pages[i].first << " " << _error_pages[i].second << " ";
            std::cout << std::endl;
            std::cout << "mime_types : ";
            for (size_t i = 0; i < _mime_types.size(); i++)
                std::cout << _mime_types[i] << " ";
            std::cout << std::endl;
        }
};


class Config
{
    private:
        std::vector<Server> _Servers;
        std::string _FilePath;
        std::ifstream _Configfile;
    public:
        Config();
        Config(std::string FilePath);
        ~Config();
        void parse_config();
        std::vector<Server> & getServers(){
            return _Servers;
        }
};

#endif