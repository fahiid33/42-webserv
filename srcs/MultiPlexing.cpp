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

int reader(unsigned char *buffer, Socket &client)
{
    int rc;
    bzero(buffer, 1000);
    if((rc = read(client.getSocket_fd(), buffer, 999)) < 0)
    {
        perror("  read() failed");
        client.setClose_conn(1);
        return false;
    }
    if (rc == 0) 
    {
        client.setClose_conn(1);
        return false;
    }
    buffer[999] = '\0';
    client.getrequest().insert(client.getrequest().end(), buffer, buffer + rc);
    client.getReq().setStarted(time(NULL));
    return true;
}

void initializeRequest(Socket & client)
{
    try
    {
        std::cout << "initializeRequest" << std::endl;
        Request req(client.getrequest());
        client.setReq(req);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        // basic header error handling
        // set the error status to handle the response later on prepare response
        client.get_Resp().setStatusCode(atoi(e.what()));
        client.setread_done(1);
    }
}

void MultiPlexing::handleReadData(std::pair <Socket, Server> & client)
{
    unsigned char buffer[1000];
    int rc = 0;
    std::vector<unsigned char> pattern;
    pattern.clear();
    pattern.push_back('0');
    pattern.push_back('\r');
    pattern.push_back('\n');
    pattern.push_back('\r');
    pattern.push_back('\n');

    if (!reader(buffer, client.first))
        return ;
    //search for the end of the header
/*1*/std::vector<unsigned char>::iterator pos = std::search(client.first.getrequest().begin(), client.first.getrequest().end(), pattern.begin()+1, pattern.end());

    if (pos != client.first.getrequest().end())
    {
        if (!client.first.getReq().getHeaders().empty())
            // get the body
            client.first.getReq().setBody(std::vector<unsigned char>(pos + 4, client.first.getrequest().end()));
            //----can be optimized 1 & 2 & 3
        else
        // if the header not inizialized yet
            initializeRequest(client.first);

        // perhaps the body is in the first chunk so we check if the body is complete, also if it another chunk we perform the same check
        if (!client.first.getReq().getHeaders().empty() && client.first.getReq().getHeaders().find("Content-Length") != client.first.getReq().getHeaders().end())
        {
            // check if the body exceed the max length in the server...

            if (client.first.getReq().getBody().size() >= stoi(client.first.getReq().getHeaders().find("Content-Length")->second))
            {
                if (client.first.getReq().getBody().size() > stoi(client.first.getReq().getHeaders().find("Content-Length")->second))
                    client.first.get_Resp().setResp(std::make_pair("HTTP/1.1 404 Bad Request\r\n\r\n", 32));
                client.first.setread_done(1);
            }
            else
                client.first.setread_done(0);
            return ;
        }

        // if the body is chunked we check handle it in a function, just perform some modifs on body after reading it

        if (!client.first.getReq().getHeaders().empty() && client.first.getReq().getHeaders().find("Transfer-Encoding") != client.first.getReq().getHeaders().end())
        {
            std::cout << "again" << std::endl;
            if ((pos = std::search(client.first.getrequest().begin(), client.first.getrequest().end(), pattern.begin(), pattern.end()) ) != client.first.getrequest().end())
            {
                if (pos + 5 != client.first.getrequest().end())
                {
                    client.first.get_Resp().setResp(std::make_pair("HTTP/1.1 404 Bad Request\r\n\r\n", 32));
                }
                std::cout << "here6" << std::endl;
                client.first.getReq().parseChunkedBody(client.first.getReq().getBody());
                client.first.setread_done(1);
            }
            else
                client.first.setread_done(0);
            return ;
        }

        // if the body is not chunked neither with Content-Lenght and the header is complete we set the read_done flag to 1
        // GET and DELETE methods generally don't have a body: no chunked, no Content-Length
        if (client.first.getReq().getMethod() != "" && client.first.getReq().getMethod() != "POST")
            client.first.setread_done(1);
    }
    // if the header is not complete we set the read_done flag to 0
    else
        client.first.setread_done(0);
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
        rc = write(sock.getSocket_fd() , sock.get_Resp().toString().c_str(), sock.get_Resp().toString().length());
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
            sock.setWrite_done(1);
        return ;
    }
    if ((sock.get_Resp().getOffset() + 1024) > stoi(sock.get_Resp().getHeaders().find("Content-Length")->second) && sock.get_Resp().getFile() != "")
    {
        read(sock.get_Resp().getFd(), buffer, stoi(sock.get_Resp().getHeaders().find("Content-Length")->second) - sock.get_Resp().getOffset());
        rc = write(sock.getSocket_fd() , buffer , stoi(sock.get_Resp().getHeaders().find("Content-Length")->second) - sock.get_Resp().getOffset());
        sock.get_Resp().setOffset(sock.get_Resp().getOffset() + rc);
        std::cout << GREEN << "file " << sock.get_Resp().getFile() << " done" << std::endl;
        sock.setWrite_done(1);
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
        sock.setWrite_done(1);
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
            for (int i = 0; i < clients.size(); i++)
            {
                std::cout << clients[i].first.getReq().getConn() << " " << clients[i].first.getReq().getTimeOut() << " " << time(NULL) - clients[i].first.getReq().getStarted() << std::endl;
                if ((clients[i].first.getClose_conn() || !clients[i].first.getReq().getConn() || (clients[i].first.getReq().getConn() &&
                (time(NULL) - clients[i].first.getReq().getStarted() >= clients[i].first.getReq().getTimeOut()))))
                {
                    std::cout << "clian sala mn read" << std::endl;
                    // remove the socket fd from the sets : handle error "bad file descriptor"
                    FD_CLR(clients[i].first.getSocket_fd(), &io.writefds);
                    FD_CLR(clients[i].first.getSocket_fd(), &io.readfds);
                    std::cout << RED << "close socket" << clients[i].first.getSocket_fd() << std::endl;
                    close(clients[i].first.getSocket_fd());
                    clients[i].first.clear();
                    clients.erase(clients.begin() + i);
                    i--;
                }
            }
            printf("select() == %d timed out.  End program.\n", rc);
            continue;
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
                    clients[i].first.getrequest().clear();
                    FD_SET(clients[i].first.getSocket_fd(), &io.writefds);
                }
            }

            if (FD_ISSET(clients[i].first.getSocket_fd(), &io.write_cpy))
            {
                clients[i].first.get_Resp().prepare_response(clients[i].first.getReq(), clients[i].second);
                handleWriteData(clients[i].first);
                if (clients[i].first.getWrite_done())
                {
                    clients[i].first.setWrite_done(0);
                    FD_CLR(clients[i].first.getSocket_fd(), &io.writefds);
                    // FD_CLR(clients[i].first.getSocket_fd(), &io.readfds);
                    // close(clients[i].first.getSocket_fd());
                    if (clients[i].first.get_Resp().getFile() != "")
                        close(clients[i].first.get_Resp().getFd());
                    clients[i].first.clear();
                    // clients.erase(clients.begin() + i);
                }
            }
            if (clients[i].first.getClose_conn())
            {
                FD_CLR(clients[i].first.getSocket_fd(), &io.readfds);
                FD_CLR(clients[i].first.getSocket_fd(), &io.writefds);
                close(clients[i].first.getSocket_fd());
                if (clients[i].first.get_Resp().getFile() != "" && clients[i].first.get_Resp().getFd() > 0)
                    close(clients[i].first.get_Resp().getFd());
                clients[i].first.clear();
                clients.erase(clients.begin() + i);
                i--;
            }
        }
    }
}

void    MultiPlexing::handleNewConnection(Server & server, Clients & clients)
{
    int new_socket;
    struct sockaddr_in address = server.getServerSocket().getAddress();
    int addrlen = sizeof(address);

    if ((new_socket = accept(server.getServerFd(), (struct sockaddr *)& address, (socklen_t*)&addrlen))<0)
    {
        if (errno != EWOULDBLOCK)
        {
            perror("  accept() failed");
            server.setEndServer(1);
        }
    }
    if ((fcntl(new_socket, F_SETFL, O_NONBLOCK)) <  0)
    {
        perror("In fcntl");
        exit(EXIT_FAILURE);
    }
    std::cout << "New connection , socket fd is " << new_socket << " , ip is : " << inet_ntoa(address.sin_addr) << " , port : " << ntohs(address.sin_port) << std::endl;
    FD_SET(new_socket, &io.readfds);
    if (new_socket > max_sd)
        max_sd = new_socket;
    clients.push_back(std::make_pair(Socket(new_socket, address), server));
    clients.back().first.clear();
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
    this->clients.push_back(std::make_pair(Socket(sock, address), server));
}
