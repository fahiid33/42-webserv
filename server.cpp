#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>      // std::ifstream


#define PORT 2222
int main()
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length:";
    
    std::ifstream file;
    file.open("index.html");
    std::string str;
    std::getline(file, str, '\0');
    hello += std::to_string(str.length());
    hello += "\n\n";
    hello += str;
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        perror("In socket");
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
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        printf("-------------------From client---------------------------\n");
        printf("%s\n",buffer );
        printf("-------------------From you---------------------------\n");
        // std::getline(std::cin, hello);
        write(new_socket , hello.c_str() , strlen(hello.c_str()));
        printf("--------------------------------------------\n");
        close(new_socket);
    }
    return 0;
}