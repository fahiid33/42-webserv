#include "request.hpp"

Request::Request()
{
}

Request::Request(std::string request)
{
    this->request = request;
}

Request::~Request()
{
}

std::string Request::getRequest()
{
    return this->request;
}

void Request::setRequest(std::string request)
{
    this->request = request;
}