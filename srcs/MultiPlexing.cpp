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

void initializeRequest(std::pair <Socket, Server> & client, std::vector<Server> & servers)
{
    // sleep(1000);

    try
    {
        Request req(client, servers);
        client.first.setReq(req);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        // basic header error handling
        // set the error status to handle the response later on prepare response
        client.first.get_Resp().generateErrorPage(atoi(e.what()), Location());
        client.first.get_Resp().setStatusCode(atoi(e.what()));
        client.first.setread_done(1);
    }
}

void MultiPlexing::handleReadData(std::pair <Socket, Server> & client, std::vector<Server> & servers)
{
    unsigned char buffer[1000];

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
    std::vector<unsigned char>::iterator pos = std::search(client.first.getrequest().begin(), client.first.getrequest().end(), pattern.begin()+1, pattern.end());

    if (pos != client.first.getrequest().end())
    {
        if (!client.first.getReq().getHeaders().empty())
            client.first.getReq().setBody(std::vector<unsigned char>(pos + 4, client.first.getrequest().end()));
        else
            initializeRequest(client, servers);

        // perhaps the body is in the first chunk so we check if the body is complete, also if it another chunk we perform the same check
        if (!client.first.getReq().getHeaders().empty() && client.first.getReq().getHeaders().find("Content-Length") != client.first.getReq().getHeaders().end())
        {
            // check if the body exceed the max length in the server...

            if (client.first.getReq().getBody().size() >= strtoul(client.first.getReq().getHeaders().find("Content-Length")->second.c_str(), NULL, 10))
            {
                if (client.first.getReq().getBody().size() > strtoul(client.first.getReq().getHeaders().find("Content-Length")->second.c_str(), NULL, 10))
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
            if ((pos = std::search(client.first.getrequest().begin(), client.first.getrequest().end(), pattern.begin(), pattern.end()) ) != client.first.getrequest().end())
            {
                if (pos + 5 != client.first.getrequest().end())
                {
                    client.first.get_Resp().setResp(std::make_pair("HTTP/1.1 404 Bad Request\r\n\r\n", 32));
                }
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
    int rc = 0;
    sock.getReq().setStarted(time(NULL));
    sock.setWrite_done(0);
    if (!sock.get_Resp().getIsOpen())
    {
        if (sock.getReq().getConn())
            sock.get_Resp().setHeader("Connection", "Keep-Alive");
        else
            sock.get_Resp().setHeader("Connection", "close");
        rc = write(sock.getSocket_fd() , sock.get_Resp().toString().c_str(), sock.get_Resp().toString().length());
        if (sock.get_Resp().getFile() != "")
        {
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
    }
    else if ((sock.get_Resp().getOffset() + 1024) > strtoul(sock.get_Resp().getHeaders().find("Content-Length")->second.c_str(), NULL, 10) && sock.get_Resp().getFile() != "")
    {
        read(sock.get_Resp().getFd(), buffer, stoi(sock.get_Resp().getHeaders().find("Content-Length")->second) - sock.get_Resp().getOffset());
        rc = write(sock.getSocket_fd() , buffer , stoi(sock.get_Resp().getHeaders().find("Content-Length")->second) - sock.get_Resp().getOffset());
        sock.get_Resp().setOffset(sock.get_Resp().getOffset() + rc);
        sock.setWrite_done(1);
        if (sock.get_Resp().getFd() > 0)
        {
            if (close(sock.get_Resp().getFd()) != 0)
                perror("close");
        }
    }
    else if (sock.get_Resp().getFile() != "")
    {
        read(sock.get_Resp().getFd(), buffer, 1024);
        rc = write(sock.getSocket_fd() , buffer, 1024);
        sock.get_Resp().setOffset(sock.get_Resp().getOffset() + rc);
    }
    if (rc <= 0)
    {
        std::cerr << "Error writing to client socket" << std::endl;
        sock.setWrite_done(1);
    }
}



void MultiPlexing::setup_server(Servers & servers)
{
    int rc;
    Clients clients;

    for (Servers::iterator it = servers.begin(); it != servers.end(); it++) {
        CreateServerSockets((it->second)[0]);
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
            for (size_t i = 0; i < clients.size(); i++)
            {
                if ((clients[i].first.getClose_conn() || !clients[i].first.getReq().getConn() || (clients[i].first.getReq().getConn() &&
                (time(NULL) - clients[i].first.getReq().getStarted() >= clients[i].first.getReq().getTimeOut())))) {
                    // remove the socket fd from the sets : handle error "bad file descriptor"
                    FD_CLR(clients[i].first.getSocket_fd(), &io.writefds);
                    FD_CLR(clients[i].first.getSocket_fd(), &io.readfds);
                    close(clients[i].first.getSocket_fd());
                    clients[i].first.clear();
                    clients.erase(clients.begin() + i);
                    i--;
                }
            }
            std::cout << YELLOW << "++++++++++++++ waiting for new connection +++++++++++++++" << std::endl;
            continue;
        }
        // close the connection if the client is not responding
        // Check for new connections
        for (Servers::iterator it = servers.begin(); it != servers.end(); it++) {
            if (FD_ISSET((it->second)[0].getServerFd(), &io.read_cpy))
                handleNewConnection((it->second)[0], clients);
        }
        // Handle client I/O
        for (size_t i = 0; i < clients.size(); i++)
        {
            if (FD_ISSET(clients[i].first.getSocket_fd(), &io.read_cpy)) {
                handleReadData(clients[i], servers[clients[i].second.getPort()]);
                if (!clients[i].first.getClose_conn() && clients[i].first.getread_done()) {
                    clients[i].first.getrequest().clear();
                    FD_SET(clients[i].first.getSocket_fd(), &io.writefds);
                }
            }

            if (FD_ISSET(clients[i].first.getSocket_fd(), &io.write_cpy)) {
                if (clients[i].first.get_Resp().getStatusCode() == 0)
                    clients[i].first.get_Resp().prepare_response(clients[i].first.getReq(), clients[i].second);
                handleWriteData(clients[i].first);
                if (clients[i].first.getWrite_done()) {
                    clients[i].first.setWrite_done(0);
                    FD_CLR(clients[i].first.getSocket_fd(), &io.writefds);
                    clients[i].first.clear();
                }
            }
            if (clients[i].first.getClose_conn()) {
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
        if (errno != EWOULDBLOCK) {
            perror("  accept() failed");
            server.setEndServer(1);
        }
    }
    std::cout << GREEN << "++++++++++++++ connection aceepeted " << new_socket << " on : " << server.getPort()  << " ++++++++++++++" << std::endl;
    if ((fcntl(new_socket, F_SETFL, O_NONBLOCK)) <  0) {
        perror("In fcntl");
        exit(EXIT_FAILURE);
    }
    FD_SET(new_socket, &io.readfds);
    if (new_socket > max_sd)
        max_sd = new_socket;
    clients.push_back(std::make_pair(Socket(new_socket, address), server));
    clients.back().first.clear();
    usleep(500);
}

void    MultiPlexing::CreateServerSockets(Server& server)
{
    Socket sock;

    sock = server.getServerSocket();
    sock.create_sockets(server.getPort());
    server.setServerSocket(sock);
    max_sd = server.getServerFd();
    FD_SET(server.getServerFd(), &io.readfds);
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
