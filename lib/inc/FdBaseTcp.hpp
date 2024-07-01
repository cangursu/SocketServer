
#ifndef __FD_BASE_TCP_HPP__
#define __FD_BASE_TCP_HPP__

#include <string>
//#include <sys/un.h>
#include <netinet/in.h>
#include <stdint.h>


#define SOCKET_INVALID -1




struct FdBaseTcp
{
public:

    static constexpr int FD_INVALID = -1;

    FdBaseTcp();
    FdBaseTcp(const std::string &ip, uint16_t port);
    FdBaseTcp(const std::string &ip, uint16_t port, int fd);
    FdBaseTcp(int fd);

    int     Fd() const;
    void    Fd(int fd) ;

    bool    Init();
    bool    Accept(FdBaseTcp &fd);
    bool    Connect();
    bool    Listen();
    bool    Send(uint8_t buff[], size_t len);
    ssize_t Recv(uint8_t buff[], size_t len);

    bool    IsValid() const;
    void    Close();
    void    DeleteKey();

private :

    int         _fd = FD_INVALID;
    std::string _ip;
    uint16_t    _port = 0;
    sockaddr_in _addr {};

};



#endif //__FD_BASE_TCP_HPP__