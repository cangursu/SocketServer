
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
    FdBaseUdp(const std::string &ip, uint16_t port);

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

    bool    operator == (const FdBaseUdp &fd);
    bool    operator != (const FdBaseUdp &fd);

private :

    int         _fd = FD_INVALID;
    std::string _ip;
    uint16_t    _port = 0;
    sockaddr_in _addr {};
};


inline
bool FdBaseUdp::operator == (const FdBaseUdp &fd) {
    return fd._fd == _fd;
}


inline
bool FdBaseUdp::operator != (const FdBaseUdp &fd) {
    return !operator == (fd);
}



#endif //__FD_BASE_UDP_HPP__

