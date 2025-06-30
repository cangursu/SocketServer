
#ifndef __FD_BASE_UDS_HPP__
#define __FD_BASE_UDS_HPP__

#include <string>
#include <sys/un.h>
#include <stdint.h>


#define SOCKET_INVALID -1


//https://www.educative.io/answers/how-to-implement-udp-sockets-in-c

struct FdBaseUds
{
public:

    static constexpr int FD_INVALID = -1;

    FdBaseUds();
    FdBaseUds(const std::string &key);

    int     Fd() const;
    void    Fd(int fd) ;

    bool    Init();
    bool    Accept(FdBaseUds &fd);
    bool    Connect();
    bool    Listen();
    bool    Send(uint8_t buff[], size_t len);
    ssize_t Recv(uint8_t buff[], size_t len);

    bool    IsValid() const;
    void    Close();
    void    DeleteKey();

    bool    operator == (const FdBaseUds &fd);
    bool    operator != (const FdBaseUds &fd);

private :

    int         _fd = FD_INVALID;
    std::string _key;
    sockaddr_un _addr {};

};



inline
bool FdBaseUds::operator == (const FdBaseUds &fd) {
    return fd._fd == _fd;
}


inline
bool FdBaseUds::operator != (const FdBaseUds &fd) {
    return !operator == (fd);
}



#endif //__FD_BASE_UDS_HPP__