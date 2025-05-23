
#ifndef __FD_BASE_UDP_HPP__
#define __FD_BASE_UDP_HPP__

#include <string>
//#include <sys/un.h>
#include <netinet/in.h>
#include <stdint.h>


#define SOCKET_INVALID -1




struct FdBaseUdp
{
public:

    static constexpr int FD_INVALID = -1;

    FdBaseUdp();
    FdBaseUdp(uint16_t port);
    FdBaseUdp(uint16_t port, int fd);

    int     Fd() const;
    void    Fd(int fd) ;

    bool    Init();
    bool    Accept(FdBaseUdp &fd);
    bool    Connect();
    bool    Listen();
    bool    Send(uint8_t buff[], size_t len);
    ssize_t Recv(uint8_t buff[], size_t len);

    bool    IsValid() const;
    void    Close();
    void    DeleteKey();

private :

    int         _fd = FD_INVALID;

    uint16_t    _port = 0;
    sockaddr_in _addr {};
    sockaddr_in _addrClient {};
};



#endif //__FD_BASE_UDP_HPP__

