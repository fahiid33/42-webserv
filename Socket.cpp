#include "request.hpp"

Socket::Socket()
{
    _sockaddr.sin_family = AF_INET;
    _sockaddr.sin_addr.s_addr = INADDR_ANY;
    _sockaddr.sin_port = htons(PORT);
    addrlen = sizeof(_sockaddr);
}

Socket::~Socket()
{
}

int Socket::getSocket_fd()
{
    return s_fd;
}

struct sockaddr_in  & Socket::getSockaddr()
{
    return _sockaddr;
}

void Socket::setSocket_fd(int socket_fd)
{
    s_fd = socket_fd;
}

void Socket::setSockaddr(struct sockaddr_in  &sockaddr)
{
    _sockaddr = sockaddr;
}

void    create_socket(void)
{
    Socket sock;
    int server_fd, new_socket;
    struct sockaddr_in address = sock.getSockaddr();
    int     on;

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
    if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        perror("In fcntl");
        exit(EXIT_FAILURE);
    }
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
    sock.setSocket_fd(server_fd);
}