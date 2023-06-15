#pragma once

#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>

class Request {
    
    private:
        std::string request;
        
    public:
        Request();
        Request(std::string request);
        ~Request();
        std::string getRequest();
        void setRequest(std::string request);
        Request &operator=(const Request &request){
            this->request = request.request;
            return *this;
        };
};
