

#include "FdBaseUdp.hpp"

#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>



FdBaseUdp::FdBaseUdp()
{
}


FdBaseUdp::FdBaseUdp(const std::string &ip, uint16_t port)
    : _ip(ip)
    , _port(port)
{
}


int FdBaseUdp::Fd() const
{
    return _fd;
}


void FdBaseUdp::Fd(int fd)
{
    _fd = fd;
}


sockaddr_in FdBaseUdp::Addr() const
{
    return _addr;
}


void FdBaseUdp::Addr(const sockaddr_in &addr)
{
    _addr = addr;
}


bool FdBaseUdp::IsValid() const
{
    return (FD_INVALID != _fd);
}


void FdBaseUdp::DeleteKey()
{
    //unlink(_key.data());
}


void FdBaseUdp::Close()
{
    if ( IsValid() )
    {
        ::shutdown(_fd, SHUT_RDWR);
        ::close(_fd);
    }
    _fd = FD_INVALID;
}


bool FdBaseUdp::Init()
{
    _fd = ::socket(AF_INET, SOCK_DGRAM, 0/*IPPROTO_UDP*/);
    if (false  == IsValid())
        return false;

    int optval = 1;
    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));

    int buff = 8 * 1024 * 1024; // 8MB
    setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &buff, sizeof(buff));


    // Set None Blocking
    int flags = 0;
    if (flags = fcntl(_fd, F_GETFL, 0); -1 == flags) {
        return false;
    }
    if (-1 == fcntl(_fd, F_SETFL, flags | O_NONBLOCK)) {
        return false;
    }


    std::memset(&_addr, '\0', sizeof(_addr));
    _addr.sin_family = AF_INET;
    if (_ip.empty())
        _addr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        _addr.sin_addr.s_addr = inet_addr(_ip.data());
    _addr.sin_port = htons(_port);

    return true;
}


bool FdBaseUdp::Accept(FdBaseUdp &fd)
{
    fd = *this;
    return true;
}


bool FdBaseUdp::Connect()
{
    if (IsValid() && (connect(_fd, (struct sockaddr*)&_addr, sizeof(_addr)) != -1))
        return true;
    return false;
}


bool FdBaseUdp::Send(uint8_t buff[], size_t len)
{
    socklen_t lenAddr = sizeof(_addr);
    if (sendto(_fd, buff, len, MSG_NOSIGNAL, (sockaddr*)&_addr, lenAddr) == -1)
        return false;
    return true;

}


ssize_t FdBaseUdp::Recv(uint8_t buff[], size_t len)
{
    socklen_t lenClientAddr = sizeof(_addr);
    ssize_t res = recvfrom(_fd, buff, len, 0, (sockaddr *)&_addr, &lenClientAddr);
    return res;
}


bool FdBaseUdp::Listen()
{
    if (IsValid())
    {
        if (SOCKET_INVALID == bind(_fd, (struct sockaddr*)&_addr, sizeof(_addr)))
        {
            //LOG_ERROR << "Unable to bind " << std::endl;
            //LOG_ERROR << "errno : " << errno << ", " << ErrnoText(errno) << std::endl;
            return false;
        }
    }
    return true;
}
