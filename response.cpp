#include "response.hpp"

Response::Response()
{
}

Response::Response(std::pair<std::string, u_long> response)
{
    this->response = response;
    this->offset = 0;
}

Response::~Response()
{
}

std::pair<std::string, u_long> &Response::getData()
{
    return this->response;
}

u_long &Response::getOffset()
{
    return this->offset;
}

void Response::setOffset(u_long offset)
{
    this->offset = offset;
}

void Response::setResponse(std::pair<std::string, u_long> response)
{
    this->response = response;
}

