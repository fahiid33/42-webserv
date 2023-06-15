#include "../includes/Socket.hpp"

Socket::Socket()
{
    
}

Socket::Socket(int s_fd, sockaddr_in address)
{
    this->s_fd = s_fd;
    this->close_conn = 0;
    this->address = address;
}

Socket::~Socket()
{
}

void Socket::setClose_conn(const int &close_conn)
{
    this->close_conn = close_conn;
}

void Socket::setResp(Response &resp)
{
    this->resp = resp;
}

void Socket::setReq(Request &req)
{
    this->req = req;
}

Response &Socket::get_Resp()
{
    return resp;
}

Request &Socket::getReq()
{
    return req;
}

int Socket::getClose_conn()
{
    return close_conn;
}

int Socket::getSocket_fd()
{
    return s_fd;
}

sockaddr_in Socket::getAddress()
{
    return address;
}

struct sockaddr_in   Socket::init_Sockadd()
{
    struct sockaddr_in  _sockaddr;
    _sockaddr.sin_family = AF_INET;
    _sockaddr.sin_addr.s_addr = INADDR_ANY;
    _sockaddr.sin_port = htons(PORT);
    return _sockaddr;
}

void Socket::setSocket_fd(int &socket_fd)
{
    s_fd = socket_fd;
}

void    Socket::create_sockets(void)
{
    address = this->init_Sockadd();
    int     on;

    if ((s_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(s_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
    {
        perror("In setsockopt");
        exit(EXIT_FAILURE);
    }
    // if (fcntl(s_fd, F_SETFL, O_NONBLOCK) < 0)
    // {
    //     perror("In fcntl");
    //     exit(EXIT_FAILURE);
    // }
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    if (bind(s_fd, (struct sockaddr *)&address, sizeof(address))<0) 
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(s_fd, MAXNAMLEN) < 0) 
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
}