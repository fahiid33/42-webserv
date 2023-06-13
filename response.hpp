#pragma once

#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>      // std::ifstream
#include <sstream>      // std::istringstream
#include <dirent.h>

class Response {
    
    private:
        std::pair<std::string, u_long> response;
        u_long offset;
    public:
        Response();
        Response(std::pair<std::string, u_long> response);
        ~Response();
        std::pair<std::string, u_long> &getData();
        u_long &getOffset();
        void setResponse(std::pair<std::string, u_long> response);
        void setOffset(u_long offset);
        Response &operator=(const Response &response){
            this->response = response.response;
            return *this;
        };
};