#pragma once

#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>      // std::ifstream
#include <sstream>      // std::istringstream
#include "Socket.hpp"
#include <dirent.h>

class Server
{
    private:
        Socket ser;
        fd_set master_set;
        fd_set working_set;
        fd_set write_fds;
        int max_sd;
        int end_server;
    public:
        Server();
        ~Server();
        void setMasterSet(int fd);
        fd_set& getMasterSet();
        fd_set& getWriteFds();
        fd_set& getWorkingSet();
        Socket & getServerSocket();
        int getEndServer();
        
        void setEndServer(int end_server);
        void    setServerSocket(Socket & sock);
        void setMaxSd(int sd);
        int getMaxSd();
        void setServerFd(int fd);
        int getServerFd();
};

// while (!end_server)
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