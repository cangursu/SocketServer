

#include "FdBaseUdp.hpp"

#include <sstream>
#include <cstring>
#include <sys/socket.h>
//#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>





FdBaseUdp::FdBaseUdp()
{
}

FdBaseUdp::FdBaseUdp(const std::string &ip, uint16_t port)
    : _ip(ip)
    , _port(port)
{
}

FdBaseUdp::FdBaseUdp(int fd)
    : _fd (fd)
{
}

FdBaseUdp::FdBaseUdp(const std::string &ip, uint16_t port, int fd)
    : _fd (fd)
    , _ip(ip)
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
    _fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (false  == IsValid())
        return false;

    int optval = 1;
    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(optval));

    std::memset(&_addr, '\0', sizeof(_addr));
    _addr.sin_family        = AF_INET;
    _addr.sin_port          = htons(_port);
    _addr.sin_addr.s_addr   = inet_addr(_ip.data());

    return true;
}

bool FdBaseUdp::Accept(FdBaseUdp &fd)
{
    socklen_t len = sizeof(fd._addr);
    fd._fd = accept(_fd, (sockaddr*)&(fd._addr), &len);
    return fd.IsValid();
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
    socklen_t lenClientAddr = sizeof(_addrClient);
    return recvfrom(_fd, buff, len, 0, (sockaddr *)&_addrClient, &lenClientAddr);
}


bool FdBaseUdp::Listen()
{
    if (IsValid())
    {
        // SO_REUSEADDR ???
        if (SOCKET_INVALID == bind(_fd, (struct sockaddr*)&_addr, sizeof(_addr)))
        {
            //LOG_ERROR << "Unable to bind " << std::endl;
            //LOG_ERROR << "errno : " << errno << ", " << ErrnoText(errno) << std::endl;
            return false;
        }
    }
    return true;
}
