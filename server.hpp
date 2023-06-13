#pragma once
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>      // std::ifstream
#include <sstream>      // std::istringstream
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <map>
#include <vector>
#include <signal.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "socket.hpp"

#define PORT 1112

std::string getContentType(const std::string &fileName, const std::map<std::string, std::string> &mimeTypes);

void create_socket();
