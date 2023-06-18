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
#include "request.hpp"
#include "response.hpp"



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
        
        Socket & operator=(const Socket &sock);
        
        Response &get_Resp();
        Request &getReq();
        int getClose_conn();
        int getSocket_fd();
        
        struct sockaddr_in getAddress();
        struct sockaddr_in   init_Sockadd(int port);
        
        void setClose_conn(const int close_conn);
        void setSocket_fd(int &socket_fd);
        void setResp(Response &resp);
        void setReq(Request &req);
        void create_sockets(int port);
};


