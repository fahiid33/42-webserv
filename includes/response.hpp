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
// #include "server.hpp"

class Server;

class Response
{
    private:
    std::map<std::string, std::string> mime_types;
    std::string _content_type;
    std::pair<std::string, u_long> _resp;
    u_long _offset;
    public:
    Response();
    Response(Request & req, Server & server);
    ~Response();
    u_long getOffset();
    void setOffset(u_long offset);
    std::pair<std::string, u_long> &getResp();
    void setResp(const std::pair<std::string, u_long> &resp);
    std::string getContentType(const std::string& file , std::map<std::string, std::string>& mime_t);
    std::map<std::string, std::string> mime_types_init();
    std::string auto_indexing(const char *path);
};

std::pair<std::string, u_long> prepare_response(Request & req, Server & server);