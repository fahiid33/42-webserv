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

#define PORT 1113

class Socket{
    private:
        int s_fd;
        struct sockaddr_in address;
        Response resp;
        Request req;
        int close_conn;
    public:
        Socket();
        Socket(int s_fd, struct sockaddr_in address);
        ~Socket();
        void setClose_conn(const int &close_conn);
        Response &get_Resp();
        Request &getReq();
        int getClose_conn();
        int getSocket_fd();
        struct sockaddr_in getAddress();
        struct sockaddr_in   init_Sockadd();
        void setSocket_fd(int &socket_fd);
        void setResp(Response &resp);
        void setReq(Request &req);
        void create_sockets();
        Socket & operator=(const Socket &sock){
            this->s_fd = sock.s_fd;
            this->address = sock.address;
            return *this;
        };
};


