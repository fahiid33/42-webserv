#include "../includes/Multiplexing.hpp"

MultiPlexing::MultiPlexing()
{
}

MultiPlexing::~MultiPlexing()
{
}

// void    MultiPlexing::setup_server(void)
// {
//     Socket sock;
//     Response response;
//     int new_socket;
//     int max_sd, desc_ready, end_server = 0, close_conn = 0;
//     std::pair<std::string, u_long> resp;
//     fd_set master_set, working_set;
//     fd_set write_fds;
//     int rc;
//     std::istringstream iss;
//     std::string hello;
//     int server_fd = create_sockets();
//     struct sockaddr_in address = sock.init_Sockadd();
//     int    addrlen = sock.get_addrlen();
//     FD_ZERO(&master_set);
//     max_sd = server_fd;
//     FD_SET(server_fd, &master_set);
//     while (!end_server)
//     {
//             memcpy(&working_set, &master_set, sizeof(master_set));
//             if ((rc = select(max_sd + 1, &working_set, &write_fds, NULL, NULL)) < 0)
//             {
//                 perror("select() failed");
//                 exit(EXIT_FAILURE);
//             }
//             desc_ready = rc;
//             for (int i = 0; i <= max_sd && desc_ready > 0; i++)
//             {
//                 if (FD_ISSET(i, &working_set))
//                 {
//                     desc_ready -= 1;
//                     if (i == server_fd)
//                     {
//                         do
//                         {
//                             printf("\n+++++++ Waiting for new connection ++++++++\n\n");
//                             if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
//                             {
//                                 if (errno != EWOULDBLOCK)
//                                 {
//                                     perror("  accept() failed");
//                                     end_server = 1;
//                                 }
//                                 break;
//                             }
//                             rc = fcntl(server_fd, F_SETFL, O_NONBLOCK);
//                             if (rc < 0)
//                             {
//                                 perror("In fcntl");
//                                 exit(EXIT_FAILURE);
//                             }
//                             // std::cout << "new_socket = " << new_socket << std::endl;
//                             // std::cout << "max_sd = " << max_sd << std::endl;
//                             // std::cout << "server_fd = " << server_fd << std::endl;
//                             FD_SET(new_socket, &master_set);
//                             if (new_socket > max_sd)
//                                 max_sd = new_socket;
//                         }
//                         while (new_socket != -1);
//                     }
//                     else
//                     {
//                         char buffer[30000];
//                         close_conn = 0;
//                         int ff = 0;
//                         resp.second = 0;
//                         while (1)
//                         {
//                             bzero(buffer, 30000);
//                             if ((rc = read(i, buffer, sizeof(buffer))) < 0)
//                             {
//                                 perror("  read() failed");
//                                 close_conn = 1;
//                                 break;
//                             }
//                             if (rc == 0)
//                             {
//                                 std::cout << "  Connection closed" << std::endl;
//                                 close_conn = 1;
//                                 break;
//                             }
//                             std::string str(buffer);
//                             std::string skip;
//                             iss.str(str);
//                             iss >> skip;
//                             iss >> hello;
//                             if (hello.cend()[-1] == '/')
//                             {
//                                 if (hello == "/")
//                                     hello = response.auto_indexing("./");
//                                 else
//                                 {
//                                     hello = hello.substr(1, hello.length() - 2);
//                                     hello = response.auto_indexing(hello.c_str());
//                                 }
//                             }
//                             else
//                             {
//                                 if (hello == "/")
//                                     resp = prepare_response("index.html", hello.substr(1,(hello.find_last_not_of('/'))).c_str());
//                                 else
//                                 {
//                                     hello = hello.substr(1, hello.length() - 1);
//                                     resp = prepare_response(hello.c_str(), hello.substr(0,(hello.find_last_of('/'))).c_str());
//                                 }
//                             }
//                             iss.clear();
//                             while (1)
//                             {
//                                 rc = write(i , resp.first.c_str() + ff , 1024);
//                                 ff += rc;
//                                 if (rc == -1)
//                                 {
//                                     FD_CLR(i, &write_fds);
//                                     FD_SET(i, &working_set);
//                                 }
//                                 else if (ff >= resp.second)
//                                 {
//                                     FD_CLR(i, &write_fds);
//                                     FD_SET(i, &working_set);
//                                     break;
//                                 }
//                                 if (rc < 0)
//                                 {
//                                     perror("  write() failed");
//                                     close_conn = 1;
//                                     break;
//                                 }
//                             }
//                         }
//                         if(close_conn)
//                         {
//                             close(i);
//                             FD_CLR(i, &master_set);
//                             if (i == max_sd)
//                             {
//                                 while (FD_ISSET(max_sd, &master_set) == 0)
//                                 max_sd -= 1;
//                             }
//                         }
//                 }
//             }
//         }
    
//     }
//     for (int i=0; i <= max_sd; ++i)
//     {
//         if (FD_ISSET(i, &master_set))
//             close(i);
//     }
// }




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

        std::string skip;
        iss.str(str);
        iss >> skip;
        iss >> hello;
        
        // if (hello.cend()[-1] == '/') {
        //     if (hello == "/") {
        //         hello = resp.auto_indexing("./");
        //     }
        //     else {
        //         hello = hello.substr(1, hello.length() - 2);
        //         hello = resp.auto_indexing(hello.c_str());
        //     }
        // }
        if (hello == "/") {
            Sock.get_Resp().getResp() = prepare_response("index.html", hello.substr(1, hello.find_last_not_of('/')).c_str());
            Sock.get_Resp().setResp(prepare_response("index.html",
                hello.substr(1, hello.find_last_of('/')).c_str()));
        }
        else {
            hello = hello.substr(1, hello.length() - 1);
            Sock.get_Resp().setResp(prepare_response(hello.c_str(),
                hello.substr(0, hello.find_last_of('/')).c_str()));
        }
        iss.clear();
}


void MultiPlexing::handleWriteData(Socket &sock) 
{
    //  handling client data writing
    // Get the appropriate response data to send back to the client
    // std::cout << "offset = " << sock.get_Resp().getOffset() << sock.get_Resp().getResp().first << std::endl;
    int rc = write(sock.getSocket_fd() , sock.get_Resp().getResp().first.c_str() + sock.get_Resp().getOffset(), 1024);
    // std::cout << "offset = " << sock.get_Resp().getOffset() << std::endl;
    sock.get_Resp().setOffset(sock.get_Resp().getOffset() + rc);
    if (sock.get_Resp().getOffset() >= sock.get_Resp().getResp().second)
    {
        // std::cout << "t9ra kaml n salina = " << sock.get_Resp().getOffset() << std::endl;
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
    else
    {
        // std::cout << "t9ra msh kaml = " << sock.get_Resp().getOffset() << std::endl;
    }
    if (sock.get_Resp().getOffset() < sock.get_Resp().getResp().second)
    {
        std::cout << rc << "  " << sock.get_Resp().getResp().second - sock.get_Resp().getOffset()<< std::endl;
    }
}


void    MultiPlexing::handleNewConnection(Server & server, std::vector<Socket> &Sock)
{
    int new_socket;
    struct sockaddr_in address = server.getServerSocket().getAddress();
    int addrlen = sizeof(address);
    // do
    // {
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
        // std::cout << "new_socket = " << new_socket << std::endl;
        // std::cout << "max_sd = " << max_sd << std::endl;
        // std::cout << "server_fd = " << server_fd << std::endl;
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
            printf("select() timed out.  End program.\n");
        }
        if (FD_ISSET(server.getServerFd(), &server.getWorkingSet()))
        {
            handleNewConnection(server, clian);
        }
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
    
    // for (int i = 0; i <= max_sd; ++i) {
    //     if (FD_ISSET(i, &master_set))
    //         close(i);
    // }
