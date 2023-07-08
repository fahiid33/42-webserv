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
class Location;


class Response
{
    private:
        std::map<std::string, std::string> mime_types;
        std::string _content_type;
        int _status_code;
        int fd;
        std::string file;
        int is_open;
        std::pair<std::string, u_long> _resp;
        u_long _offset;
        std::map<std::string, std::string> headers;
        std::map<int, std::string> errorMessages;

    public:
        Response();
        Response(const Response &resp);
        Response(Request & req, Server & server);
        ~Response();

        std::string toString() const;
        void setHeader(const std::string& key, const std::string& value);
        void generateErrorPage(int code, Location const & loc);
        void initErrorMessages();
        std::map<std::string, std::string> &getHeaders();
        void clear();
        Response & operator=(const Response &resp);
        u_long getOffset();
        void setOffset(u_long offset);
        int getIsOpen();
        int getStatusCode();
        void setIsOpen(int is_open);
        void setFd(int fd);
        void setStatusCode(int status_code);
        int getFd();
        std::string getFile();
        void setFile(std::string file);
        std::pair<std::string, u_long> &getResp();
        void setResp(const std::pair<std::string, u_long> &resp);
        std::string getContentType(const std::string& file , std::map<std::string, std::string>& mime_t);
        std::map<std::string, std::string> mime_types_init();
        void    auto_indexing(const char *path);
        void    HandleGet(Request & req, Location & loc);
        void    HandlePost(Request & req, Location & loc);
        void    HandleDelete(Request & req, Location & loc);
        void    prepare_response(Request & req, Server & server);
        std::vector<Location>::iterator  match_loc(Server & server, std::string const & path);
        int write_file_in_path(Location &client, std::vector<unsigned char> content, std::string path);
        int deleteDirectory(const char *path);
};
