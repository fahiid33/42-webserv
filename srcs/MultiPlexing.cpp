#include "../includes/Multiplexing.hpp"

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

void MultiPlexing::handleReadData(std::pair <Socket, Server> & client)
{
    char buffer[10002];
    int rc = 0;
    std::istringstream iss;
    std::string hello;
    bzero(buffer, 10000);
    // std::cout << "socket read from it " << client.first.getSocket_fd() << std::endl;
    if ((rc = read(client.first.getSocket_fd(), buffer, sizeof(buffer))) < 0) 
    {
        perror("  read() failed");
        client.first.setClose_conn(1);
        return ;
    }
    
    if (rc == 0) 
    {
        std::cout << "  Connection closed" << std::endl;
        client.first.setClose_conn(1);
        return ;
    }
    // client.second.print_server();
    std::string str(buffer);

    try
    {
        Request req(str);
        client.first.get_Resp().prepare_response(req, client.second);
        // requested the file
        std::cout << RED << "file: " << client.first.get_Resp().getFile() << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        exit(1);
    }   
}


void MultiPlexing::handleWriteData(Socket &sock) 
{
    //  handling client data writing
    // Get the appropriate response data to send back to the client
    char buffer[1025];
    bzero(buffer, 1025);
    int rc ;
    if (!sock.get_Resp().getIsOpen())
    {
        rc = write(sock.getSocket_fd() , sock.get_Resp().getResp().first.c_str(), sock.get_Resp().getResp().first.length());
        if (sock.get_Resp().getFile() != "")
        {
            sock.get_Resp().setFd(open(sock.get_Resp().getFile().c_str(), O_RDONLY));
            if (sock.get_Resp().getFd() < 0)
            {
                perror("open");
                sock.setClose_conn(1);
                return ;
            }
            sock.get_Resp().setIsOpen(1);
        }
        return ;
    }
    if ((sock.get_Resp().getOffset() + 1024) > sock.get_Resp().getResp().second && sock.get_Resp().getFile() != "")
    {
        read(sock.get_Resp().getFd(), buffer, sock.get_Resp().getResp().second - sock.get_Resp().getOffset());
        rc = write(sock.getSocket_fd() , buffer , sock.get_Resp().getResp().second - sock.get_Resp().getOffset());
        sock.get_Resp().setOffset(sock.get_Resp().getOffset() + rc);
        std::cout << GREEN << "file " << sock.get_Resp().getFile() << " done" << std::endl;
        sock.setClose_conn(1);
    }
    else if (sock.get_Resp().getFile() != "")
    {
        read(sock.get_Resp().getFd(), buffer, 1024);
        rc = write(sock.getSocket_fd() , buffer, 1024);
        sock.get_Resp().setOffset(sock.get_Resp().getOffset() + rc);
    }
    
    if (rc < 0)
    {
        perror("  write() failed");
        sock.setClose_conn(1);
        return ;
    }

}


void    MultiPlexing::handleNewConnection(Server & server, std::vector<std::pair <Socket, Server> > & clients)
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
    // std::cout << "new_socket = " << new_socket << std::endl;
    FD_SET(new_socket, &io.readfds);
    if (new_socket > max_sd)
        max_sd = new_socket;
    clients.push_back(std::make_pair(Socket(new_socket, address), server));
}

void MultiPlexing::setup_server(std::vector<Server> &servers) 
{
    Socket sock;
    int new_socket;
    int desc_ready, close_conn = 0;
    std::vector<std::pair <Socket, Server> > clients;
    int rc;

    for (int i = 0; i < servers.size(); i++)
    {
        sock = servers[i].getServerSocket();
        sock.create_sockets(servers[i].getPort());
        servers[i].setServerSocket(sock);
        max_sd = servers[i].getServerFd();
        FD_SET(servers[i].getServerFd(), &io.readfds);
        std::cout << "port: " << servers[i].getPort() << " " << max_sd << std::endl;
    }
    while (1)
    {
        struct timeval timeout;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        memcpy(&io.read_cpy, &io.readfds, sizeof(io.readfds));
        memcpy(&io.write_cpy, &io.writefds, sizeof(io.writefds));
        if ((rc = select(max_sd + 1, &io.read_cpy, &io.write_cpy, NULL, &timeout)) < 0) {
            perror("select() failed");
            exit(EXIT_FAILURE);
        }
        if (rc == 0)
        {
            // printf("select() timed out.  End program.\n");
            continue;
        }

        for (int j = 0; j < servers.size(); j++)
        {
            if (FD_ISSET(servers[j].getServerFd(), &io.read_cpy))
                handleNewConnection(servers[j], clients);
        }
        // if (servers[j].getEndServer())
        // {
        //     close(servers[j].getServerFd());
        //     servers.erase(servers.begin() + j);
        // }
        for (int i = 0; i < clients.size(); i++)
        {
            if (FD_ISSET(clients[i].first.getSocket_fd(), &io.read_cpy))
            {
                handleReadData(clients[i]);
                if (!clients[i].first.getClose_conn())
                {
                    FD_SET(clients[i].first.getSocket_fd(), &io.writefds);
                    FD_CLR(clients[i].first.getSocket_fd(), &io.readfds);
                }
                else
                {
                    FD_CLR(clients[i].first.getSocket_fd(), &io.readfds);
                    close(clients[i].first.getSocket_fd());
                    close(clients[i].first.get_Resp().getFd());
                    clients[i].first.setClose_conn(0);
                    clients.erase(clients.begin() + i);
                    i--;
                }
            }
        }
        for (int i = 0; i < clients.size(); i++)
        {
            if (FD_ISSET(clients[i].first.getSocket_fd(), &io.write_cpy)) 
            {
                handleWriteData(clients[i].first);
                if (clients[i].first.getClose_conn())
                {
                    std::cout << YELLOW << "yes dead " << clients[i].first.get_Resp().getFile() << std::endl;
                    FD_CLR(clients[i].first.getSocket_fd(), &io.writefds);
                    close(clients[i].first.getSocket_fd());
                    close(clients[i].first.get_Resp().getFd());
                    clients[i].first.setClose_conn(0);
                    clients.erase(clients.begin() + i);
                    i--;
                }
            }
        }
    }
}