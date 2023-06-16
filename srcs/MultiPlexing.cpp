#include "../includes/Multiplexing.hpp"

MultiPlexing::MultiPlexing()
{
}

MultiPlexing::~MultiPlexing()
{
}

void MultiPlexing::handleReadData(Socket &Sock)
{
    char buffer[30000];
    int rc = 0;
    std::istringstream iss;
    std::string hello;

    bzero(buffer, 30000);
        
        if ((rc = read(Sock.getSocket_fd(), buffer, sizeof(buffer))) < 0) 
        {
            perror("  read() failed");
            Sock.setClose_conn(1);
            return ;
        }
        
        if (rc == 0) 
        {
            std::cout << "  Connection closed" << std::endl;
            Sock.setClose_conn(1);
            return ;
        }
        
        std::string str(buffer);
        iss.str(str);
        try
        {
            Request req(str);
            Sock.get_Resp().setResp(prepare_response(req.getFile().c_str(),req.getPath().c_str()));
            iss.clear();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            exit(1);
        }
        

        // if (hello.cend()[-1] == '/') {
        //     if (hello == "/") {
        //         hello = resp.auto_indexing("./");
        //     }
        //     else {
        //         hello = hello.substr(1, hello.length() - 2);
        //         hello = resp.auto_indexing(hello.c_str());
        //     }
        // }
        
}


void MultiPlexing::handleWriteData(Socket &sock) 
{
    //  handling client data writing
    // Get the appropriate response data to send back to the client
    int rc ;
    if (sock.get_Resp().getResp().second < 1024)
    {
        rc = write(sock.getSocket_fd() , sock.get_Resp().getResp().first.c_str(),
            sock.get_Resp().getResp().second);
        sock.setClose_conn(1);
    }
    else
    {
        rc = write(sock.getSocket_fd() , sock.get_Resp().getResp().first.c_str() + sock.get_Resp().getOffset(), 1024);
        sock.get_Resp().setOffset(sock.get_Resp().getOffset() + rc);
    }
    std::cout << "rc = " << rc << std::endl;
    if (sock.get_Resp().getOffset() >= sock.get_Resp().getResp().second)
    {
        sock.get_Resp().setOffset(0);
        sock.setClose_conn(1);
        return ;
    }
    if (rc < 0)
    {
        perror("  write() failed");
        sock.setClose_conn(1);
        return ;
    }

}


void    MultiPlexing::handleNewConnection(Server & server, std::vector<Socket> &Sock)
{
    int new_socket;
    struct sockaddr_in address = server.getServerSocket().getAddress();
    int addrlen = sizeof(address);

    printf("\n+++++++ Waiting for new connection ++++++++\n\n");
    if ((new_socket = accept(server.getServerFd(), (struct sockaddr *)& address, (socklen_t*)&addrlen))<0)
    {
        if (errno != EWOULDBLOCK)
        {
            perror("  accept() failed");
            server.setEndServer(1);
        }
        // break;
    }
    std::cout << "Connection accepted" << std::endl;
    if ((fcntl(new_socket, F_SETFL, O_NONBLOCK)) <  0)
    {
        perror("In fcntl");
        exit(EXIT_FAILURE);
    }
    std::cout << "new_socket = " << new_socket << std::endl;
    FD_SET(new_socket, &server.getMasterSet());
    if (new_socket > server.getMaxSd())
        server.setMaxSd(new_socket);
    Sock.push_back(Socket(new_socket, address));
}

void MultiPlexing::setup_server() 
{
    Socket sock;
    int new_socket;
    int desc_ready, close_conn = 0;
    std::vector<Socket> clian;
    int rc;
    Server server;
    fd_set read_cpy, write_cpy;
    sock.create_sockets();
    server.setServerSocket(sock);
    server.setMaxSd(server.getServerFd());
    server.setMasterSet(server.getServerFd());
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    
    while (!server.getEndServer())
    {
        memcpy(&server.getWorkingSet(), &server.getMasterSet(), sizeof(server.getMasterSet()));
        memcpy(&write_cpy, &server.getWriteFds(), sizeof(server.getWriteFds()));
        if ((rc = select(server.getMaxSd() + 1, &server.getWorkingSet(), &write_cpy, NULL, &timeout)) < 0) {
            perror("select() failed");
            exit(EXIT_FAILURE);
        }
        if (rc == 0)
        {
            // printf("select() timed out.  End program.\n");
        }
        if (FD_ISSET(server.getServerFd(), &server.getWorkingSet()))
            handleNewConnection(server, clian);
        for (int i = 0; i < clian.size(); i++)
        {
            if (FD_ISSET(clian[i].getSocket_fd(), &server.getWorkingSet()))
            {
                handleReadData(clian[i]);
                if (clian[i].getClose_conn())
                {
                    FD_CLR(clian[i].getSocket_fd(), &server.getMasterSet());
                    close(clian[i].getSocket_fd());
                    clian.erase(clian.begin() + i);
                }
                else
                {
                    FD_SET(clian[i].getSocket_fd(), &server.getWriteFds());
                    FD_CLR(clian[i].getSocket_fd(), &server.getMasterSet());
                }
            }
        }
        for (int i = 0; i < clian.size(); i++)
        {
            if (FD_ISSET(clian[i].getSocket_fd(), &write_cpy)) 
            {
                handleWriteData(clian[i]);
                if (clian[i].getClose_conn())
                {
                    FD_CLR(clian[i].getSocket_fd(), &server.getWriteFds());
                    close(clian[i].getSocket_fd());
                    clian.erase(clian.begin() + i);
                }
                else if (clian[i].get_Resp().getOffset() == 0)
                {
                    std::cout << "offsetttt = " << std::endl;
                    FD_CLR(clian[i].getSocket_fd(), &server.getWriteFds());
                    close(clian[i].getSocket_fd());
                    clian.erase(clian.begin() + i);
                }
            }
        }

    }
}