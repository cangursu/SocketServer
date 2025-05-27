
#include "FdBaseUds.hpp"

#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>




FdBaseUds::FdBaseUds()
{
}

FdBaseUds::FdBaseUds(const std::string &key)
    : _key(key)
{
}

int FdBaseUds::Fd() const
{
    return _fd;
}

void FdBaseUds::Fd(int fd)
{
    _fd = fd;
}

bool FdBaseUds::IsValid() const
{
        return (FD_INVALID != _fd);
}

void FdBaseUds::Close()
{
    if ( IsValid() )
    {
        ::shutdown(_fd, SHUT_RDWR);
        ::close(_fd);
    }
    _fd = FD_INVALID;
}


void FdBaseUds::DeleteKey()
{
    unlink(_key.data());
}


bool FdBaseUds::Init()
{
    _fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (false  == IsValid())
        return false;

    int optval = 1;
    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(optval));

    std::memset(&_addr, '\0', sizeof(_addr));
    _addr.sun_family = AF_UNIX;
    std::strncpy(_addr.sun_path, _key.data(), sizeof(_addr.sun_path) - 1);

    return true;
}

bool FdBaseUds::Accept(FdBaseUds &fd)
{
    socklen_t len = sizeof(fd._addr);
    fd._fd = accept(_fd, (sockaddr*)&(fd._addr), &len);
    return fd.IsValid();
}


bool FdBaseUds::Connect()
{
    if (IsValid() && (connect(_fd, (struct sockaddr*)&_addr, sizeof(_addr)) != -1))
        return true;
    return false;
}


bool FdBaseUds::Send(uint8_t buff[], size_t len)
{
    if (send(_fd, buff, len, MSG_NOSIGNAL) == -1)
        return false;
    return true;

}

ssize_t FdBaseUds::Recv(uint8_t buff[], size_t len )
{
    return recv(_fd, buff, len, 0);
}


bool FdBaseUds::Listen()
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