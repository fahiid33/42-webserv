#pragma once

#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream> 
#include <sstream>  
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <map>
#include <signal.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "request.hpp"
#include "response.hpp"


#define PORT 1112

class Socket{
    private:
        int s_fd;
        struct sockaddr_in  _sockaddr;
        int     addrlen;
        Request _request;
        Response _response;
    public:
        Socket();
        Socket(int socket_fd) : s_fd(socket_fd) {};
        ~Socket();
        int getSocket_fd();
        struct sockaddr_in  & getSockaddr();
        Request &getRequest();
        Response &getResponse();
        void setSocket_fd(int socket_fd);
        void setRequest(Request request);
        void setResponse(Response response);
        void setSockaddr(struct sockaddr_in  &sockaddr);
};

// void create_socket();