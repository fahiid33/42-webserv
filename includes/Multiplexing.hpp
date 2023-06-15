#pragma once

#include <vector>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include "server.hpp"

class Socket;
class Response;

class MultiPlexing
{
    Server server;

public:
    MultiPlexing();
    ~MultiPlexing();

    void setup_server();
    void handleNewConnection(Server &server, std::vector<Socket> &Sock);
    void handleReadData(Socket &pdo);
    void handleWriteData(Socket &sock);
};
