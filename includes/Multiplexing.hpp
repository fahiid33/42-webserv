#pragma once


#include "server.hpp"
#include "socket.hpp"
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


typedef struct IOsets
{
    fd_set readfds;
    fd_set writefds;
    fd_set read_cpy;
    fd_set write_cpy;
} IOsets;

class MultiPlexing
{
    Server server;
    IOsets io;
    int max_sd;


public:
    MultiPlexing();
    ~MultiPlexing();

    void setup_server(std::vector<Server> &servers);
    int getMaxSd();
    void setMaxSd(int max_sd);
    void handleNewConnection(Server & server, std::vector<std::pair <Socket, Server>> & clients);

    void handleReadData(std::pair <Socket, Server> &);
    void handleWriteData(Socket &sock);
};
