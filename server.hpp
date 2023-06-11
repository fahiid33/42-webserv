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

#define PORT 1117

std::string getContentType(const std::string &fileName, const std::map<std::string, std::string> &mimeTypes);

void create_socket();
