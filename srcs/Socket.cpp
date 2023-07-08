
#include "../includes/socket.hpp"

Socket::Socket() : s_fd(-1), address(), resp(Response()), req(Request()), close_conn(0), write_done(0), read_done(0), request(), already_bind(0)
{
}

Socket::Socket(int s_fd, sockaddr_in address)
{
    this->write_done = 0;
    this->read_done = 0;
    this->already_bind = 0;
    this->s_fd = s_fd;
    this->close_conn = 0;
    this->request.clear();
    this->address = address;
    this->resp = Response();
    this->req = Request();
}

Socket::~Socket()
{
    this->write_done = 0;
    this->close_conn = 0;
    this->read_done = 0;
    this->request.clear();
    this->already_bind = 0;
    this->s_fd = -1;
    this->resp.clear();
    this->req.clear();
}

void Socket::clear()
{
    this->write_done = 0;
    this->close_conn = 0;
    this->read_done = 0;
    this->request.clear();
    this->already_bind = 0;
    this->resp.clear();
    this->req.clear();
}

Socket & Socket::operator=(const Socket &sock)
{
    this->write_done = sock.write_done;
    this->close_conn = sock.close_conn;
    this->already_bind = sock.already_bind;
    this->s_fd = sock.s_fd;
    this->request = sock.request;
    this->address = sock.address;
    this->resp = sock.resp;
    this->req = sock.req;
    return *this;
}

void Socket::setWrite_done(int d)
{
    this->write_done = d;
}

void Socket::setread_done(int d)
{
    this->read_done = d;
}

void Socket::setrequest(std::vector<unsigned char> req)
{
    this->request = req;
}

std::vector<unsigned char> &Socket::getrequest()
{
    return this->request;
}

int Socket::getread_done()
{
    return this->read_done;
}
int Socket::getWrite_done()
{
    return this->write_done;
}

void Socket::setClose_conn(const int close_conn)
{
    this->close_conn = close_conn;
}

int Socket::getAlready_bind()
{
    return already_bind;
}

void Socket::setResp(Response const &resp)
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

struct sockaddr_in   Socket::init_Sockadd(int port)
{
    struct sockaddr_in  _sockaddr;
    _sockaddr.sin_family = AF_INET;
    _sockaddr.sin_addr.s_addr = INADDR_ANY;
    _sockaddr.sin_port = htons(port);
    return _sockaddr;
}

void Socket::setSocket_fd(int &socket_fd)
{
    s_fd = socket_fd;
}

void Socket::setAlreadyBind(int already_bind)
{
    this->already_bind = already_bind;
}

void    Socket::create_sockets(int port)
{
    address = this->init_Sockadd(port);
    int     on;

    if ((s_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(s_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0) {
        perror("In setsockopt");
        exit(EXIT_FAILURE);
    }
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    if (bind(s_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("In bind");
        already_bind = 1;
        exit(EXIT_FAILURE);
    } if (listen(s_fd, MAXNAMLEN) < 0) {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
}