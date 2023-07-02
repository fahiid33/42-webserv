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
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "server.hpp"

class Server;

class Config
{
    private:
        std::unordered_map < int, std::vector<Server> > _Servers;
        std::string _FilePath;
        std::ifstream _Configfile;
    public:
        Config();
        Config(std::string FilePath);
        ~Config();
        void parse_config();
        std::unordered_map < int, std::vector<Server> > & getServers(){
            return _Servers;
        }
};
