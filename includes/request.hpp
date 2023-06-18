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

class Request {
    
    private:
        std::string request;
        std::string method;
        std::string path;
        std::string file;
        std::string version;
        std::string host;
        std::string conn;
        std::string tr_enc;
        std::string body;
        int         content_length;

    public:
        Request();
        Request(const Request &request);
        Request(std::string request);
        ~Request();
        Request &operator=(const Request &request);
        
        std::string getMethod();
        std::string getPath();
        std::string getVersion();
        std::string getHost();
        std::string getFile();
        std::string getConn();
        int         getContent_length();
        std::string getTr_enc();
        std::string getRequest();
        void setRequest(std::string request);
};
