#include "server.hpp"



std::map<std::string, std::string> getMime_types(void)
{
    std::map<std::string, std::string> mimeTypes;
    std::ifstream file("mime.types");
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string extension;
        std::string contentType;
        char a;
        if (iss >> contentType >> a >> extension) {
            mimeTypes[extension] = contentType;\
        }
    }

    return mimeTypes;
}

std::string getContentType(const std::string& fileName, const std::map<std::string, std::string>& mimeTypes) {
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string extension = fileName.substr(dotPos + 1);
        const std::map<std::string, std::string>::const_iterator it = mimeTypes.find(extension);
        if (it != mimeTypes.end()) {
            return it->second;
        }
    }
    return "application/octet-stream";
}


std::string auto_indexing(const char *dir)
{
    std::string resp = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length:";
    std::string str = "<html><head><title>Index of /</title></head><body><h1>Index of /</h1><hr><pre>";
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir)) == NULL) 
    {
        resp = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 13\n\n404 not found";
        return resp;
    }
    while ((dirp = readdir(dp)) != NULL) 
    {
        str += "<a href=\"";
        str += dirp->d_name;
        str += "\">";
        str += dirp->d_name;
        str += "</a>\n";
    }
    str += "</pre><hr></body></html>";
    resp += std::to_string(str.length());
    resp += "\n\n";
    resp += str;
    return resp;
}

std::pair<std::string, u_long> prepare_response(const char *file_name,const char *dir)
{
    std::pair<std::string, u_long> resp ;
    std::map<std::string, std::string> mimeTypes = getMime_types();
    std::string contentType = getContentType(file_name, mimeTypes);
    resp.first = "HTTP/1.1 200 OK\nContent-Type: ";
    resp.first += contentType;
    std::ifstream file;
    file.open(file_name, std::ios::binary | std::ios::ate);
    if (!file.is_open() || access(dir, R_OK) == -1)
    {
        //check if the file exist
        if (access(file_name, F_OK) == -1)
        {    
            resp.first = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 13\n\n404 not found";
            resp.second = 13;
            return resp;
        }
        std::cout << "dir ="<< dir << std::endl;
        //check if the file has read permission
        if (access(file_name, R_OK) == -1 || access(dir, R_OK) == -1)
        {
            resp.first = "HTTP/1.1 403 Forbidden\nContent-Type: text/html\nContent-Length: 15\n\n403 forbidden";
            resp.second = 15;
            return resp;
        }
    }
    std::string str;
    resp.first += "\nContent-Length:";
    resp.second = file.tellg();
    file.seekg(0, std::ios::beg);
    resp.first += std::to_string(resp.second);
    resp.first += "\n\n";
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    resp.first += content;
    return resp;
}



void    create_socket()
{
    int server_fd, new_socket, max_sd;
    struct sockaddr_in address;
    int    desc_ready, end_server = 0;
    int addrlen = sizeof(address);
    std::vector<Socket> clients;
    std::istringstream iss;
    std::string hello;
    int close_conn = 0;
    std::pair<std::string, u_long> resp;
    fd_set master_set, working_set;
    fd_set write_fds;
    int rc, on = 1;

   
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
    {
        perror("In setsockopt");
        exit(EXIT_FAILURE);
    }

    rc = fcntl(server_fd, F_SETFL, O_NONBLOCK);
    if (rc < 0)
    {
        perror("In fcntl");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, MAXNAMLEN) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    FD_ZERO(&master_set); //set the master set to empty master_set
    max_sd = server_fd;
    FD_SET(server_fd, &master_set); // dkhl server_fd f master_set
    while(!end_server)
    {
        // std::cout << "----------------------------------------------------------------------------------------" << std::endl;
        // copy master_set to working_set. to avoid changing master_set while using select
        memcpy(&working_set, &master_set, sizeof(master_set));
        // configure the timeout
        struct timeval timeout;
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        // wait for an activity on one of the sockets
        rc = select(max_sd + 1, &working_set, &write_fds, NULL, &timeout);
        if (rc < 0)
        {
            perror("  select() failed");
            // std::cout << "select() failed" << errno << std::endl;
            // std::cout << std::endl;
            // std::cout << std::endl;
            exit(32);
        }
        if (rc == 0)
        {
            std::cout << "select() timed out. no socket is ready." << std::endl;
            continue;
        }
        // one or more descriptors are readable
        desc_ready = rc;
        for (int i = server_fd; i <= max_sd && desc_ready > 0; i++)
        {
            close_conn = 0;
            // the socket is ready for reading
            desc_ready -= 1;
            if (FD_ISSET(i, &working_set))
            {
                if (i == server_fd)
                // check if there is a new connection (client)
                {
                    std::cout << "listening socket " << i << " is readable" <<  std::endl;
                    do{
                        std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n" << std::endl; 
                        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
                        {
                            if (errno != EWOULDBLOCK)
                            {
                                perror("  accept() failed");
                                end_server = 1;
                            }
                            break;
                        }
                        rc = fcntl(server_fd, F_SETFL, O_NONBLOCK);
                        if (rc < 0)
                        {
                            perror("In fcntl");
                            std::cout << "9lwa() failed" << errno << std::endl;
                            std::cout << std::endl;
                            std::cout << std::endl;
                            exit(21);
                        }
                        std::cout << "new_socket = " << new_socket << std::endl;
                        // add the new connection (client) to master_set
                        FD_SET(new_socket, &master_set);
                        clients.push_back(Socket(new_socket));
                        if (new_socket > max_sd)
                            max_sd = new_socket;
                    }while (new_socket != -1);
                }
                // check if there is a new data (request) from a client

                else if (i > server_fd && (i - server_fd - 1) < clients.size())
                {
                    char buffer[30000];

                    std::cout << "descriptor " << i << " is readable" <<  std::endl;
                    close_conn = 0;
                    resp.second = 0;
                    bzero(buffer, 30000);
                    rc = read(i, buffer, 30000);
                    if (rc < 0)
                    {
                        perror("read() failed");
                        close_conn = 1;
                    }
                    if (rc == 0)
                    {
                        std::cout << "  Connection closed" << std::endl;
                        close_conn = 1;
                    }
                    // add the socket to write_fds to send the response
                    FD_SET(i, &write_fds);
                    std::string str(buffer);
                    std::string skip;
                    iss.str(str);
                    iss >> skip;
                    iss >> hello;
                    clients[i - server_fd - 1].setRequest(Request(str));
                    std::cout << str << std::endl;
                    if (hello.cend()[-1] == '/')
                    {
                        if (hello == "/")
                            hello = auto_indexing("./");
                        else
                        {
                            hello = hello.substr(1, hello.length() - 2);
                            hello = auto_indexing(hello.c_str());
                        }
                    }
                    else
                    {
                        if (hello == "/")
                            resp = prepare_response("index.html", hello.substr(1,(hello.find_last_not_of('/'))).c_str());
                        else
                        {
                            hello = hello.substr(1, hello.length() - 1);
                            resp = prepare_response(hello.c_str(), hello.substr(0,(hello.find_last_of('/'))).c_str());
                        }
                    }
                    clients[i - server_fd - 1].setResponse(Response(resp));
                    iss.clear();
                }
            }
            // the socket is ready for writing
            if (FD_ISSET(i, &write_fds) && (i - server_fd - 1) < clients.size())
            {
                // std::cout << "client req" << clients[i - server_fd - 1].getResponse().getOffset() << std::endl;
                // sleep(10);
                std::cout << "descriptor " << i << " is writable want write: " <<  clients[i - server_fd - 1].getResponse().getData().second << " bytes" <<  std::endl;
                rc = write(i , clients[i - server_fd - 1].getResponse().getData().first.c_str() + clients[i - server_fd - 1].getResponse().getOffset(), 1024);
                clients[i - server_fd - 1].getResponse().getOffset() += rc;
                if (rc == -1)
                {
                    std::cout << "t9ra kaml 1 salina = " << clients[i - server_fd - 1].getResponse().getOffset() << std::endl;
                    FD_CLR(i, &write_fds);
                    clients[i - server_fd - 1].getResponse().getOffset() = 0;
                    close_conn = 1;
                }
                else if (clients[i - server_fd - 1].getResponse().getOffset() >= clients[i - server_fd - 1].getResponse().getData().second)
                {
                    std::cout << "t9ra kaml n salina = " << clients[i - server_fd - 1].getResponse().getOffset() << std::endl;
                    FD_CLR(i, &write_fds);
                    clients[i - server_fd - 1].getResponse().getOffset() = 0;
                    close_conn = 1;
                }
                else
                {
                    std::cout << "t9ra msh kaml = " << clients[i - server_fd - 1].getResponse().getOffset() << std::endl;
                }
                // std::cout << "clients[i - server_fd - 1].getResponse().getData().second= " << resp.first.c_str() << std::endl;
                if (rc < 0)
                {
                    perror("  write() failed");
                    close_conn = 1;
                }
                if (clients[i - server_fd - 1].getResponse().getOffset() < clients[i - server_fd - 1].getResponse().getData().second)
                {
                    std::cout << rc << "  " << clients[i - server_fd - 1].getResponse().getData().second - clients[i - server_fd - 1].getResponse().getOffset()<< std::endl;
                }
            }
            if (close_conn)
            {
                close(i);
                std::cout << "closing socket " << i << std::endl;
                FD_CLR(i, &master_set);
                if (i == max_sd)
                {
                    while (FD_ISSET(max_sd, &master_set) == 0)
                        max_sd -= 1;
                }
            }
        }
    }
    perror("failed: ");
    std::cout << "closing all sockets" << std::endl;
    for (int i=0; i <= max_sd; ++i)
    {
        if (FD_ISSET(i, &master_set))
            close(i);
    }
}