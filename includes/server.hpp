#pragma once

#include  <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/select.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <netinet/in.h> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <map>
#include <vector>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "socket.hpp"
#include "parse_conf.hpp"
#include "cgi.hpp"


class Location
{
    private:
        std::string                                         _locationNumber;
        std::string                                         _locationPath;
        std::vector<std::string>                            _allowedMethods;
        std::string                                         _root;
        std::vector<std::string>                            _index;
        bool                                                _autoIndex;
        size_t                                              _clientMaxBodySize;
        std::string                                         _uploadPath;
        std::pair<std::string, std::string>                 _redirection;

    public:
        Location();
        ~Location();

        std::string                                         & getLocationNumber();
        std::string                                         & getLocationPath();
        std::vector<std::string>                            & getAllowedMethods();
        
        std::string                                         & getRoot();
        std::vector<std::string>                            & getIndex();
        bool                                                & getAutoIndex();
        std::string                                         & getUploadPath();
        std::pair<std::string, std::string>                 & getRedirection();
        size_t                                              & getClientMaxBodySize();

        void                                                setLocationNumber(std::string locationNumber);
        void                                                setLocationPath(std::string locationPath);
        void                                                setAllowedMethods(std::vector<std::string> allowedMethods);
        void                                                setRoot(std::string root);
        void                                                setIndex(std::vector<std::string> index);
        void                                                setAutoIndex(bool autoIndex);
        void                                                setUploadPath(std::string uploadPath);
        void                                                setRedirection(std::pair<std::string, std::string> redirection);
        void                                                setClientMaxBodySize(size_t clientMaxBodySize);

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
            std::cout << "uploadPath : " << _uploadPath << std::endl;
            // std::cout << std::endl;
            std::cout << "redirection : " << _redirection.first << " " << _redirection.second << std::endl;
            std::cout << "clientMaxBodySize : " << _clientMaxBodySize << std::endl;
        }
};


class Server
{
    private:
        Socket ser;
        fd_set master_set;
        fd_set working_set;
        fd_set write_cpy;
        fd_set write_fds;
        size_t _port;
        std::string _host;

        std::vector<std::string> _serverNames;
        std::vector<Location> _locations;
        std::vector<std::pair<size_t, std::string> > _error_pages;
        std::string _root;
        std::vector<std::string> _index;
        int max_sd;
        int end_server;
        Cgi  _cgi;
        
    public:
        Server();
        ~Server();
        void setMasterSet(int fd);
        fd_set& getMasterSet();
        fd_set& getWriteFds();
        fd_set& getWriteCpy();
        fd_set& getWorkingSet();
        Socket & getServerSocket();
        Cgi & get_cgi();

        int getEndServer();
        size_t & getPort();
        std::vector<std::string> & getServerNames();
        std::vector<Location> & getLocations();
        std::string & getHost();
        std::vector<std::pair<size_t, std::string> > & getError_pages();
        int getServerFd();
        std::string & getRoot();
        std::vector<std::string>    & getIndex();
        int getMaxSd();

        void    set_cgi(Cgi cgi);
        void    setEndServer(int end_server);
        void    setServerSocket(Socket & sock);
        void    setMaxSd(int sd);
        void    setServerFd(int fd);
        void    setPort(size_t port);
        void    setServerNames(std::vector<std::string> serverNames);
        void    setLocations(std::vector<Location> locations);
        void    setHost(std::string host);
        void    setRoot(std::string root);
        void    setIndex(std::vector<std::string> index);
        void    setError_pages(std::vector<std::pair<size_t, std::string> > error_pages);
        void    print_server(){
            std::cout << "port : " << _port << std::endl;
            std::cout << "host : " << _host << std::endl;
            std::cout << "root : " << _root << std::endl;
            std::cout << "index : ";
            for (size_t i = 0; i < _index.size(); i++)
                std::cout << _index[i] << " ";
            std::cout << std::endl;
            std::cout << "serverNames : ";
            for (size_t i = 0; i < _serverNames.size(); i++)
                std::cout << _serverNames[i] << " ";
            std::cout << std::endl;
            std::cout << "cgi : ";
            std::cout << _cgi.get_Cgi().first << " ";
            std::cout << _cgi.get_Cgi().second << std::endl;
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
        }
};
