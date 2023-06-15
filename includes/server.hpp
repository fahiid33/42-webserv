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

// while (1)
//     {
//         readcpy = io.fdread;
//         writecpy = io.fdwrite;
//         int r = select(io._fdmax + 1, &readcpy, &writecpy, NULL, &vl);
//         if (r == 0)
//             continue;
//         else
//         {
//             for (size_t j = 0; j < servers.size(); j++)
//             {
//                 int fdserver = servers[j].getSocket().getSocketFd();
//                 if (FD_ISSET(fdserver, &readcpy))
//                 {
//                     Client newC;
//                     sockaddr_in client_addr;
//                     socklen_t client_addr_size = sizeof(client_addr);
//                     if ((fd_client = accept(fdserver, (sockaddr *)&client_addr,&client_addr_size)) != -1)
//                     {
//                         fcntl(fd_client, F_SETFL, O_NONBLOCK);
//                         newC.setSocketFd(fd_client);
//                         newC.setServer(servers[j]);
//                         ClientRequest.push_back(std::pair<Client, Request>(newC, Request()));
//                         std::cout << MAGENTA << "New client "<< inet_ntoa(client_addr.sin_addr) << ":" << servers[j].getPort() << " connected" << RESET << std::endl;
//                         io.setFdRead(fd_client);
//                         if (fd_client > io._fdmax)
//                             io._fdmax = fd_client;
//                     }
//                 }
//             }
//             for (u_int i = 0; i < ClientRequest.size(); i++)
//             {
//                 if (FD_ISSET(ClientRequest[i].first.getSocketFd(), &readcpy)) // request
//                 {
//                     char request[10002];
//                     int r = recv(ClientRequest[i].first.getSocketFd(), request, 10000, 0); 
//                     if (r == -1)
//                     {
//                         FD_CLR(ClientRequest[i].first.getSocketFd(), &io.fdread);
//                         close(ClientRequest[i].first.getSocketFd());
//                         ClientRequest.erase(ClientRequest.begin() + i);
//                     }
//                     else if (r == 0)
//                     {
//                         FD_CLR(ClientRequest[i].first.getSocketFd(), &io.fdread);
//                         close(ClientRequest[i].first.getSocketFd());
//                         ClientRequest.erase(ClientRequest.begin() + i);
//                     }
//                     else
//                     {
//                          if (ClientRequest[i].second.get_ok() == 0)
//                             ClientRequest[i].second = Request();
//                         ClientRequest[i].second.setLength(r);
//                         ClientRequest[i].second.handle_request(request);
//                          if (ClientRequest[i].second.getFinished() == 1)
//                         {
//                             std::cout << BLUE << "Request "<< trim(ClientRequest[i].second.Getrequest()["Method"]) <<
//                             " " << trim(ClientRequest[i].second.Getrequest()["Path"]) << " " <<
//                             trim(ClientRequest[i].second.Getrequest()["Version"]) << RESET << std::endl;

//                             bool found = 0;
//                             Response resp;
//                             FD_CLR(ClientRequest[i].first.getSocketFd(), &io.fdread);
//                             FD_SET(ClientRequest[i].first.getSocketFd(), &io.fdwrite);
//                             for (size_t j = 0; j < servers.size(); j++)
//                             {
//                                 for (size_t k = 0; k < servers[j].getServerNames().size(); k++)
//                                 {
//                                     if (servers[j].getServerNames()[k] == strtrim(ClientRequest[i].second.Getrequest().at("Host")))
//                                     {
//                                         resp = Response(ClientRequest[i].second, servers[j], ClientRequest[i].first.getSocketFd());
//                                         ReadyResponse.push_back(resp);
//                                         found = 1;
//                                         break;
//                                     }
//                                 }
//                             }
//                             if (!found)
//                             {
//                                 for (size_t j = 0; j < servers.size(); j++)
//                                 {
//                                     if (!servers[j].getServerNames().size() && servers[j].getPort() == ClientRequest[i].first.getServer().getPort())
//                                     {
//                                         resp = Response(ClientRequest[i].second, servers[j], ClientRequest[i].first.getSocketFd());
//                                         ReadyResponse.push_back(resp);
//                                         found = 1;
//                                         break;
//                                     }
//                                 }
//                             }
//                             if (!found)
//                             {
//                                 resp = Response(ClientRequest[i].second, ClientRequest[i].first.getServer(), ClientRequest[i].first.getSocketFd());
//                                 ReadyResponse.push_back(resp);
//                             }
//                         }
//                     }
//                 }
//             }  
//             for (u_int i = 0; i < ReadyResponse.size(); i++)
//             {
//                 if (FD_ISSET(ReadyResponse[i].getClientFD(), &writecpy)) // response
//                 {
//                     ReadyResponse[i].handler(io.fdread,io.fdwrite); 
//                     if (ReadyResponse[i].get_done() == 1)
//                         ReadyResponse.erase(ReadyResponse.begin() + i);
//                 }
                 
//             }