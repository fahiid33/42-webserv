#pragma once

#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Socket.hpp"
#include <dirent.h>

class Server
{
    private:
        Socket ser;
        fd_set master_set;
        fd_set working_set;
        fd_set write_fds;
        int max_sd;
        int end_server;
    public:
        Server();
        ~Server();
        void setMasterSet(int fd);
        fd_set& getMasterSet();
        fd_set& getWriteFds();
        fd_set& getWorkingSet();
        Socket & getServerSocket();
        int getEndServer();
        
        void setEndServer(int end_server);
        void    setServerSocket(Socket & sock);
        void setMaxSd(int sd);
        int getMaxSd();
        void setServerFd(int fd);
        int getServerFd();
};
