
#include "FdBaseTcp.hpp"

#include <sstream>
#include <cstring>
#include <sys/socket.h>
//#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>





FdBaseTcp::FdBaseTcp()
{
}

FdBaseTcp::FdBaseTcp(const std::string &ip, uint16_t port)
    : _ip(ip)
    , _port(port)
{
}

FdBaseTcp::FdBaseTcp(int fd)
    : _fd (fd)
{
}

FdBaseTcp::FdBaseTcp(const std::string &ip, uint16_t port, int fd)
    : _fd (fd)
    , _ip(ip)
    , _port(port)
{
}


int FdBaseTcp::Fd() const
{
    return _fd;
}

void FdBaseTcp::Fd(int fd)
{
    _fd = fd;
}


bool FdBaseTcp::IsValid() const
{
    return (FD_INVALID != _fd);
}

void FdBaseTcp::DeleteKey()
{
    //unlink(_key.data());
}

void FdBaseTcp::Close()
{
    if ( IsValid() )
    {
        ::shutdown(_fd, SHUT_RDWR);
        ::close(_fd);
    }
    _fd = FD_INVALID;
}


bool FdBaseTcp::Init()
{
    _fd = ::socket(AF_INET, SOCK_STREAM, 0);
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

bool FdBaseTcp::Accept(FdBaseTcp &fd)
{
    socklen_t len = sizeof(fd._addr);
    fd._fd = accept(_fd, (sockaddr*)&(fd._addr), &len);
    return fd.IsValid();
}


bool FdBaseTcp::Connect()
{
    if (IsValid() && (connect(_fd, (struct sockaddr*)&_addr, sizeof(_addr)) != -1))
        return true;
    return false;
}


bool FdBaseTcp::Send(uint8_t buff[], size_t len)
{
    if (send(_fd, buff, len, MSG_NOSIGNAL) == -1)
        return false;
    return true;

}

ssize_t FdBaseTcp::Recv(uint8_t buff[], size_t len )
{
    return recv(_fd, buff, len, 0);
}


bool FdBaseTcp::Listen()
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

        if (SOCKET_INVALID == listen(_fd, 128))
        {
            //LOG_ERROR << "Unable to listen " << std::endl;
            //LOG_ERROR << "errno : " << errno << ", " << ErrnoText(errno) << std::endl;
            return false;
        }
    }
    return true;
}
