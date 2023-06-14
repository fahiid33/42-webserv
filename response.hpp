#pragma once

#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>
#include <iterator>
#include <fcntl.h>
#include <unistd.h>

class Response
{
    private:
    std::map<std::string, std::string> mime_types;
    std::string _content_type;
    std::pair<std::string, u_long> _resp;
    u_long _offset;
    public:
    Response();
    ~Response();
    u_long getOffset();
    void setOffset(u_long offset);
    std::pair<std::string, u_long> &getResp();
    void setResp(const std::pair<std::string, u_long> &resp);
    std::string getContentType(const std::string& file , std::map<std::string, std::string>& mime_t);
    std::map<std::string, std::string> mime_types_init();
    // void setContentType(std::string &content_type);
    std::string auto_indexing(const char *path);
};

std::pair<std::string, u_long> prepare_response(const char *file_name,const char *dir);