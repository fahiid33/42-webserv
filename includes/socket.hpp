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
        int write_done;
        int read_done;
        std::vector<unsigned char> request;
        int already_bind;
    public:
        Socket() ;
        Socket(int s_fd, struct sockaddr_in address);
        ~Socket();
         
        Socket & operator=(const Socket &sock);
        
        Response &get_Resp();
        Request &getReq();
        int getAlready_bind();
        int getClose_conn();
        int getSocket_fd();
        
        struct sockaddr_in getAddress();
        struct sockaddr_in   init_Sockadd(int port);
        
        void setClose_conn(const int close_conn);
        void setWrite_done(int d);
        void setread_done(int d);
        void setrequest(std::vector<unsigned char> req);
        
        std::vector<unsigned char>& getrequest();
        int getread_done();
        int  getWrite_done();
        void setSocket_fd(int &socket_fd);
        void setAlreadyBind(int already_bind);
        void setResp(Response const &resp);
        void clear();
        void setReq(Request &req);
        void create_sockets(int port);
};


