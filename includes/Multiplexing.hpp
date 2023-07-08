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
    public :
        typedef std::vector< std::pair<Socket, Server> > Clients;
        typedef std::unordered_map<int, std::vector<Server> > Servers;
    private :
        // Server server;
        IOsets io;
        MultiPlexing::Clients clients;
        int max_sd;


    public:
        MultiPlexing();
        ~MultiPlexing();

        void setup_server(Servers & servers);
        int  getMaxSd();
        void setMaxSd(int max_sd);
        void handleNewConnection(Server & server, Clients & clients);

        const MultiPlexing::Clients & getCLients() const;

        void addClient(int sock, struct sockaddr_in address,Server & server);
        void    CreateServerSockets(Server& servers);  
        void setClients(Clients & client);
        void handleReadData(std::pair <Socket, Server> &client, std::vector<Server> & servers);
        void handleWriteData(Socket &sock);
};
