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
    FD_ZERO(&master_set);
    max_sd = server_fd;
    FD_SET(server_fd, &master_set);
    std::cout << "server_fd = " << server_fd << std::endl;
    while(!end_server)
    {
        // copy master_set to working_set. to avoid changing master_set while using select
        memcpy(&working_set, &master_set, sizeof(master_set));
        // wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        rc = select(max_sd + 1, &working_set, &write_fds, NULL, NULL);
        if (rc < 0)
        {
            perror("In select");
            exit(EXIT_FAILURE);
        }
        // one or more descriptors are readable
        desc_ready = rc;
        for (int i = 0; i <= max_sd && desc_ready > 0; i++)
        {
            if (FD_ISSET(i, &working_set))
            {
                desc_ready -= 1;
                if (i == server_fd)
                {
                    std::cout << "listening socket " << i << " is readable" <<  std::endl;
                    do{
                        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
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
                            exit(EXIT_FAILURE);
                        }
                        std::cout << "new_socket = " << new_socket << std::endl;
                        FD_SET(new_socket, &master_set);
                        if (new_socket > max_sd)
                            max_sd = new_socket;
                    }while (new_socket != -1);
                }
                else
                {
                    char buffer[30000];

                    // std::cout << "descriptor " << i << " is readable" <<  std::endl;
                    close_conn = 0;
                    int ff = 0;
                    resp.second = 0;
                    while(1)
                    {
                           { bzero(buffer, 30000);
                            rc = read(i, buffer, 30000);
                            if (rc < 0)
                            {
                                perror("  read() failed");
                                close_conn = 1;
                                break;
                            }
                            if (rc == 0)
                            {
                                std::cout << "  Connection closed" << std::endl;
                                close_conn = 1;
                                break;
                            }
                            std::string str(buffer);
                            std::string skip;
                            iss.str(str);
                            iss >> skip;
                            iss >> hello;
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
                            iss.clear();}
                            // std::cout << "resp.first= " << resp.first << std::endl;
                            while (1)
                            {

                            rc = write(i , resp.first.c_str() + ff , 1024);
                            ff += rc;
                            if (rc == -1)
                            {
                                std::cout << "t9ra kaml 1 salina = " << ff << std::endl;
                                FD_CLR(i, &write_fds);
                                FD_SET(i, &working_set);
                            }
                            else if (ff >= resp.second)
                            {
                                std::cout << "t9ra kaml n salina = " << ff << std::endl;
                                FD_CLR(i, &write_fds);
                                FD_SET(i, &working_set);
                                break;
                            }
                            else
                            {
                                std::cout << "t9ra msh kaml = " << ff << std::endl;
                            }
                            // std::cout << "resp.second= " << resp.first.c_str() << std::endl;
                            if (rc < 0)
                            {
                                perror("  write() failed");
                                close_conn = 1;
                                break;
                            }
                            if (ff < resp.second)
                            {
                                std::cout << rc << "  " << resp.second << std::endl;
                            }
                            }
                    }
                    if (close_conn)
                    {
                        close(i);
                        FD_CLR(i, &master_set);
                        if (i == max_sd)
                        {
                            while (FD_ISSET(max_sd, &master_set) == 0)
                                max_sd -= 1;
                        }
                    }
                }
            }

        }
    }
    for (int i=0; i <= max_sd; ++i)
    {
        if (FD_ISSET(i, &master_set))
            close(i);
    }
}
