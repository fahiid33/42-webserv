#include "../includes/Multiplexing.hpp"

const char* RED = "\033[31m";
const char* GREEN = "\033[32m";
const char* YELLOW = "\033[33m";

MultiPlexing::MultiPlexing()
{
    // initialize IO
    FD_ZERO(&io.readfds);
    FD_ZERO(&io.writefds);
    FD_ZERO(&io.read_cpy);
    FD_ZERO(&io.write_cpy);
}

MultiPlexing::~MultiPlexing()
{
}

int MultiPlexing::getMaxSd()
{
    return max_sd;
}

void MultiPlexing::setMaxSd(int max_sd)
{
    this->max_sd = max_sd;
}

const MultiPlexing::Clients & MultiPlexing::getCLients() const
{
    return clients;
}

void    MultiPlexing::setClients(Clients & client)
{
    clients = client;
}

void    MultiPlexing::addClient(int sock, struct sockaddr_in address, Server & server)
{
    // Clients clients;
    this->clients.push_back(std::make_pair(Socket(sock, address), server));
    // setClients(clients);
}


void MultiPlexing::handleReadData(std::pair <Socket, Server> & client)
{
    char buffer[1000];
    int rc = 0;
    std::istringstream iss;
    std::string hello;
    // std::cout << "socket read from it " << client.first.getSocket_fd() << std::endl;
    bzero(buffer, 1000);
    if((rc = read(client.first.getSocket_fd(), &buffer[0], sizeof(buffer))) < 0)
    {
        perror("  read() failed");
        client.first.setClose_conn(1);
        return ;
    }

    if (rc == 0) 
    {
        // std::cout << "  Connection closed" << std::endl;
        client.first.setClose_conn(1);
        return ;
    }
    buffer[rc] = '\0';
    // client.second.print_server();
    client.first.setrequest(client.first.getrequest() + buffer);
    std::cout << client.first.getrequest() << std::endl;
    client.first.getReq().setStarted(time(NULL));
    if (client.first.getrequest().find("\r\n\r\n") != std::string::npos)
    {
        std::cout << "header done" << std::endl;
        if (client.first.getReq().getContent_length() > 0)
        {
            if (client.first.getReq().getBody().length() >= client.first.getReq().getContent_length())
            {
                client.first.getReq().setBody(client.first.getReq().getBody() + buffer);
                std::cout << "zbi" << client.first.getReq().getBody().length() << std::endl;
                if (client.first.getReq().getBody().length() > client.first.getReq().getContent_length())
                {
                    client.first.get_Resp().setResp(std::make_pair("HTTP/1.1 404 Bad Request\r\n\r\n", 32));
                }
                client.first.setread_done(1);
            }
            else
            {
                client.first.getReq().setBody(client.first.getReq().getBody() + buffer);
                client.first.setread_done(0);
                if (client.first.getReq().getBody().length() == client.first.getReq().getContent_length())
                    client.first.setread_done(1);
            }
            return ;
        }
        else if (client.first.getReq().getTr_enc() == "chunked")
        {
            if (client.first.getrequest().find("0\r\n\r\n") != std::string::npos)
            {
                client.first.getReq().setBody(client.first.getReq().getBody() + buffer);
                if (client.first.getrequest().find("0\r\n\r\n") + 5 != client.first.getrequest().length())
                {
                    client.first.get_Resp().setResp(std::make_pair("HTTP/1.1 404 Bad Request\r\n\r\n", 32));
                }
                client.first.setread_done(1);
            }
            else
            {
                client.first.getReq().setBody(client.first.getReq().getBody() + buffer);
                client.first.setread_done(0);
            }
            return ;
        }
        else
        {
            // method
            if (client.first.getReq().getMethod() != "" && client.first.getReq().getMethod() != "POST" )
            {
                client.first.setread_done(1);
                return ;
            }
        }
        try
        {
            // std::cout << "+++++++++++++++++++++"<< client.first.getrequest() <<  " " << client.first.getSocket_fd() << rc <<client.first.getread_done() << "++++++++++++++++++++++++++++++++++++++" << std::endl;
            Request req(client.first.getrequest().c_str());

            client.first.setReq(req);

            if (client.first.getReq().getBody().length() == client.first.getReq().getContent_length() || (client.first.getReq().getMethod() != "" && client.first.getReq().getMethod() != "POST"))
            {
                std::cout << "body done" << std::endl;
                client.first.setread_done(1);
            }
            // client.first.get_Resp().prepare_response(req, client.second);
            // requested the file
            // std::cout << RED << "file: " << client.first.get_Resp().getFile() << std::endl;
        }
        catch(const std::exception& e)
        {
            if(!strcmp(e.what(), "0"))
            {
                client.first.get_Resp().setResp(std::make_pair("HTTP/1.1 404 Bad Request\r\n\r\n", 32));
            }
            else if(!strcmp(e.what(), "1"))
            {
                client.first.get_Resp().setResp(std::make_pair("HTTP/1.1 414 Request-URI Too Long\r\n\r\n", 41));
            }
            else if(!strcmp(e.what(), "2"))
            {
                client.first.get_Resp().setResp(std::make_pair("HTTP/1.1 403 Bad Request\r\n\r\n", 32));
            }
            else if(!strcmp(e.what(), "3"))
            {
                client.first.get_Resp().setResp(std::make_pair("HTTP/1.1 405 Method Not Allowed\r\n\r\n", 39));
            }
            else if(!strcmp(e.what(), "4"))
            {
                client.first.get_Resp().setResp(std::make_pair("HTTP/1.1 505 HTTP Version Not Supported\r\n\r\n", 47));
            }
            else if(!strcmp(e.what(), "5"))
            {
                client.first.get_Resp().setResp(std::make_pair("HTTP/1.1 501 Not Implemented\r\n\r\n", 36));
            }
            else if(!strcmp(e.what(), "6"))
            {
                client.first.get_Resp().setResp(std::make_pair("HTTP/1.1 500 Internal Server Error\r\n\r\n", 42));
            }
            else if(!strcmp(e.what(), "7"))
            {
                client.first.get_Resp().setResp(std::make_pair("HTTP/1.1 411 Length Required\r\n\r\n", 36));
            }
            else if(!strcmp(e.what(), "8"))
            {
                client.first.get_Resp().setResp(std::make_pair("HTTP/1.1 405 method not allowed\r\n\r\n", 39));
            }
            else if(!strcmp(e.what(), "9"))
            {
                client.first.get_Resp().setResp(std::make_pair("HTTP/1.1 402 Bad Request\r\n\r\n", 32));
            }
            else if(!strcmp(e.what(), "10"))
            {
                client.first.get_Resp().setResp(std::make_pair("HTTP/1.1 201 OK\r\n\r\n", 32));
            }
            else
            {
                std::cout << "baaad " << e.what() << std::endl;
                exit(0);
            }
            client.first.setread_done(1);
        }
    }
    else
    {
        client.first.setread_done(0);
        return ;
    }
}


void MultiPlexing::handleWriteData(Socket &sock) 
{
    //  handling client data writing
    // Get the appropriate response data to send back to the client
    char buffer[1025];
    bzero(buffer, 1025);
    int rc ;
    sock.getReq().setStarted(time(NULL));
    
    sock.setWrite_done(0);
    if (!sock.get_Resp().getIsOpen())
    {
        rc = write(sock.getSocket_fd() , sock.get_Resp().getResp().first.c_str(), sock.get_Resp().getResp().first.length());
        if (sock.get_Resp().getFile() != "")
        {
            std::cout << "file " << sock.get_Resp().getFile() << " opened" << std::endl;
            sock.get_Resp().setFd(open(sock.get_Resp().getFile().c_str(), O_RDONLY));
            if (sock.get_Resp().getFd() < 0)
            {
                perror("open");
                sock.setWrite_done(1);
                return ;
            }
            sock.get_Resp().setIsOpen(1);
        }
        else
        {
            sock.setWrite_done(1);
        }
        // std::cout << GREEN << "response header : " << sock.get_Resp().getResp().first << std::endl;
        return ;
    }
    if ((sock.get_Resp().getOffset() + 1024) > sock.get_Resp().getResp().second && sock.get_Resp().getFile() != "")
    {
        read(sock.get_Resp().getFd(), buffer, sock.get_Resp().getResp().second - sock.get_Resp().getOffset());
        rc = write(sock.getSocket_fd() , buffer , sock.get_Resp().getResp().second - sock.get_Resp().getOffset());
        sock.get_Resp().setOffset(sock.get_Resp().getOffset() + rc);
        std::cout << GREEN << "file " << sock.get_Resp().getFile() << " done" << std::endl;
        sock.setWrite_done(1);
    }
    else if (sock.get_Resp().getFile() != "")
    {
        // std::cout << GREEN << "writing...." << "still " <<sock.get_Resp().getResp().second - sock.get_Resp().getOffset() << std::endl;
        read(sock.get_Resp().getFd(), buffer, 1024);
        rc = write(sock.getSocket_fd() , buffer, 1024);
        sock.get_Resp().setOffset(sock.get_Resp().getOffset() + rc);
    }
    if (rc < 0)
    {
        perror("  write() failed");
        sock.setWrite_done(1);
        return ;
    }

}


void    MultiPlexing::handleNewConnection(Server & server, Clients & clients)
{
    int new_socket;
    struct sockaddr_in address = server.getServerSocket().getAddress();
    int addrlen = sizeof(address);

    // printf("\n+++++++ Waiting for new connection ++++++++\n\n");
    if ((new_socket = accept(server.getServerFd(), (struct sockaddr *)& address, (socklen_t*)&addrlen))<0)
    {
        if (errno != EWOULDBLOCK)
        {
            perror("  accept() failed");
            server.setEndServer(1);
        }
        // break;
    }
    // std::cout << "Connection accepted" << std::endl;
    if ((fcntl(new_socket, F_SETFL, O_NONBLOCK)) <  0)
    {
        perror("In fcntl");
        exit(EXIT_FAILURE);
    }
    std::cout << "new_socket = " << new_socket << std::endl;
    FD_SET(new_socket, &io.readfds);
    if (new_socket > max_sd)
        max_sd = new_socket;
    // this->addClient(new_socket, address, server);
    clients.push_back(std::make_pair(Socket(new_socket, address), server));

}

void    MultiPlexing::CreateServerSockets(std::vector<Server>& servers)
{
    Socket sock;

    for (int i = 0; i < servers.size(); i++)
    {
        sock = servers[i].getServerSocket();
        sock.create_sockets(servers[i].getPort());

        if (sock.getAlready_bind())
        {
            sock.setAlreadyBind(0);
            servers.erase(servers.begin() + i);
            i--;
            continue;
        }

        servers[i].setServerSocket(sock);
        max_sd = servers[i].getServerFd();
        FD_SET(servers[i].getServerFd(), &io.readfds);
    }
}

void MultiPlexing::setup_server(std::vector<Server>& servers)
{
    int rc;
    Clients clients;

    CreateServerSockets(servers);
    while (1)
    {
        struct timeval timeout;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        memcpy(&io.read_cpy, &io.readfds, sizeof(io.readfds));
        memcpy(&io.write_cpy, &io.writefds, sizeof(io.writefds));

        if ((rc = select(max_sd + 1, &io.read_cpy, &io.write_cpy, NULL, &timeout)) < 0)
        {
            perror("select() failed");
            exit(EXIT_FAILURE);
        }
        if (rc == 0)
        {
            int flag = 0;
            std::cout << max_sd << std::endl;
            for (int i = 0; i < clients.size(); i++)
            {
                std::cout << clients[i].first.getReq().getConn() << time(NULL) << clients[i].first.getReq().getStarted() << clients[i].first.getReq().getTimeOut() << std::endl;
                if ((clients[i].first.getClose_conn() || !clients[i].first.getReq().getConn() || (clients[i].first.getReq().getConn() && (time(NULL) - clients[i].first.getReq().getStarted() >= clients[i].first.getReq().getTimeOut()))))
                {
                    std::cout << "clian sala mn read" << std::endl;
                    // remove the socket fd from the sets : handle error "bad file descriptor"
                    FD_CLR(clients[i].first.getSocket_fd(), &io.writefds);
                    FD_CLR(clients[i].first.getSocket_fd(), &io.readfds);
                    std::cout << RED << "close socket" << std::endl;
                    close(clients[i].first.getSocket_fd());
                    clients[i].first.clear();
                    clients.erase(clients.begin() + i);
                    i--;
                }
                if (!clients[i].first.getWrite_done())
                    flag = 1;
            }
            if (!flag)
            {printf("select() timed out.  End program.\n");
            continue;}
        }
        // Check for new connections
        for (int j = 0; j < servers.size(); j++)
        {
            if (FD_ISSET(servers[j].getServerFd(), &io.read_cpy))
                handleNewConnection(servers[j], clients);
        }
        // Handle client I/O
        for (int i = 0; i < clients.size(); i++)
        {
            if (FD_ISSET(clients[i].first.getSocket_fd(), &io.read_cpy))
            {
                handleReadData(clients[i]);
                if (!clients[i].first.getClose_conn() && clients[i].first.getread_done())
                {
                    std::cout << "fd in write " << clients[i].first.getSocket_fd() << std::endl;
                    clients[i].first.setrequest("");
                    FD_SET(clients[i].first.getSocket_fd(), &io.writefds);
                }
            }

            if (FD_ISSET(clients[i].first.getSocket_fd(), &io.write_cpy))
            {
                if (clients[i].first.get_Resp().getResp().first == "")
                {
                    std::cout << "prepare response" << clients[i].first.getReq().getRequest() << std::endl;
                    clients[i].first.get_Resp().prepare_response(clients[i].first.getReq(), clients[i].second);
                }
                handleWriteData(clients[i].first);
                if (clients[i].first.getWrite_done())
                {
                    std::cout << "salat lktba " << clients[i].first.getSocket_fd() << std::endl;
                    clients[i].first.setWrite_done(0);
                    
                    FD_CLR(clients[i].first.getSocket_fd(), &io.writefds);
                    if (clients[i].first.get_Resp().getFile() != "")
                    {
                        close(clients[i].first.get_Resp().getFd());
                    }
                    clients[i].first.clear();
                }
            }
            if (clients[i].first.getClose_conn())
            {
                std::cout << "connection closed" << std::endl;
                FD_CLR(clients[i].first.getSocket_fd(), &io.readfds);
                FD_CLR(clients[i].first.getSocket_fd(), &io.writefds);
                // std::cout << "close socket" << std::endl;
                close(clients[i].first.getSocket_fd());
                if (clients[i].first.get_Resp().getFile() != "" && clients[i].first.get_Resp().getFd() > 0)
                {
                    close(clients[i].first.get_Resp().getFd());
                }
                clients[i].first.clear();
                clients.erase(clients.begin() + i);
                i--;
            }
        }
    }
}