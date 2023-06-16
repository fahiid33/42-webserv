#pragma once

#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <dirent.h>

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

    public:
        Request();
        Request(std::string request);
        ~Request();
        
        std::string getMethod();
        std::string getPath();
        std::string getVersion();
        std::string getHost();
        std::string getFile();
        std::string getConn();
        std::string getTr_enc();
        std::string getRequest();
        void setRequest(std::string request);
        Request &operator=(const Request &request){
            this->request = request.request;
            return *this;
        };
};
