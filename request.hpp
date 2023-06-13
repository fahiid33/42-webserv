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


#define PORT 1112

class Socket{
    private:
        int s_fd;
        struct sockaddr_in  _sockaddr;
        int     addrlen;
    public:
        Socket();
        ~Socket();
        int getSocket_fd();
        struct sockaddr_in  & getSockaddr();
        void setSocket_fd(int socket_fd);
        void setSockaddr(struct sockaddr_in  &sockaddr);
};

void create_socket();