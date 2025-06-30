
#ifndef __SOCKET_SERVER_H__
#define __SOCKET_SERVER_H__

#include <CRTPBase.hpp>

#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <array>
#include <string>
#include <atomic>
#include <cstring>
#include <zlib.h>
#include <unistd.h>
//#include <sys/socket.h>
#include <sys/epoll.h>
//#include <sys/un.h>
#include <signal.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


#define LOG_DEBUG (std::cout <<         "(" << __FILENAME__ << ":" << __LINE__ << ") ")
#define LOG_ERROR (std::cout << "ERROR : (" << __FILENAME__ << ":" << __LINE__ << ") ")
#define LOG_INFO  (std::cout <<         "(" << __FILENAME__ << ":" << __LINE__ << ") ")



enum class ESRV_RETCODE
{
    NA                  =   1,
    SUCCESS             =   0,
    ERROR_SOCKET        = - 1,
    ERROR_BIND          = - 2,
    ERROR_LISTEN        = - 3,
    ERROR_ACCEPT        = - 4,
    ERROR_NOT_CON       = - 5,
    ERROR_CONNECT       = - 6,
    ERROR_SEND          = - 7,
    ERROR_RECV          = - 8,
    ERROR_PACKET        = - 9,
    ERROR_PARAMETER     = -10,
    ERROR_EXIST         = -11,
    ERROR_PTHREAD       = -50,
    ERROR_EPOLL         = -60,
    ERROR_EPOLLWAIT     = -61,
};



const char *ErrnoText(int eno);
std::string to_string(const epoll_event &val);

void        to_string(ESRV_RETCODE val, std::ostringstream &ss);
std::string to_string(ESRV_RETCODE val);



struct Payload
{
    static constexpr uint16_t   _lenMaxPayload  = 256;//_lenMaxPacket - 10;

    uint8_t _packet [Payload::_lenMaxPayload] {};
    size_t  _len = 0;
};


//TODO: Bad Technique. Find anther way than "Dynamic Polymorphism" for multpile connetions
class PayloadImpl
{
    public:
        virtual void Payload(::Payload &) = 0;
};


void to_string(const ::Payload &pck, std::ostringstream &ss);
std::string to_string(const ::Payload &pck);



/**
 *
 *
 *
 *
*/

template <typename TImpl, typename TFdBaseSock, typename TThread>
class SocketServer
    : public TThread
    , public CRTPBase<TImpl>
{
    public:
        static constexpr uint16_t   _lenMaxPacket   = 265;
        static constexpr uint8_t    _mid[]          = "AIZ:";
        static constexpr uint16_t   _lenMid         = 4;

        SocketServer()                    = delete;
        SocketServer(const SocketServer&) = delete;
        SocketServer(SocketServer&&)      = delete;
        SocketServer &operator = (const SocketServer&)    = delete;
        SocketServer &operator = (SocketServer&&)         = delete;

        template <typename... TSockArgs>
        SocketServer(TSockArgs... args) : _fdSock(args...) {}


        ESRV_RETCODE   InitClient();
        ESRV_RETCODE   InitServer();
        void           Release(bool doUnlink = false);

        ESRV_RETCODE   SetListener();

        // Form TThread
        virtual void Run() override;

        ESRV_RETCODE    ConnectClient();
        ESRV_RETCODE    Reconnect();
        ESRV_RETCODE    Recv(Payload &packet, int fd);
        ESRV_RETCODE    Send(TFdBaseSock *client, uint8_t *payload, uint16_t lenPayload);
        ESRV_RETCODE    Start();
        void            Stop();
        ESRV_RETCODE    Wait();
        ESRV_RETCODE    ClientAccept(TFdBaseSock *fdListen, TFdBaseSock &fd);
        ESRV_RETCODE    ClientAdd(const TFdBaseSock &fd);
        ESRV_RETCODE    ClientRemove(int fd);

        TFdBaseSock &   Sock() { return _fdSock; }

//    private :

        static ssize_t  PacketParse(Payload &packet, const uint8_t buff[], const ssize_t len);
        static ssize_t  PacketCreate(uint8_t packet[], const ssize_t lenPacket, const uint8_t payload[], const uint16_t lenPayload);

    private :

        TFdBaseSock                             _fdEpoll;
        TFdBaseSock                             _fdSock;
        std::atomic<bool>                       _doExit     = false;
};


template <typename TImpl, typename TFdBaseSock, typename TThread>
void SocketServer<TImpl, TFdBaseSock, TThread>::Release(bool doUnlink /*= false*/)
{
    _fdEpoll.Close();
    _fdSock.Close();
    if (doUnlink)
        _fdSock.DeleteKey();
}


template <typename TImpl, typename TFdBaseSock, typename TThread>
ESRV_RETCODE SocketServer<TImpl, TFdBaseSock, TThread>::SetListener()
{
    return _fdSock.Init() ? ESRV_RETCODE::SUCCESS : ESRV_RETCODE::ERROR_SOCKET;
}


template <typename TImpl, typename TFdBaseSock, typename TThread>
ESRV_RETCODE SocketServer<TImpl, TFdBaseSock, TThread>::InitClient()
{
    Release(false);
    return SetListener();
}


template <typename TImpl, typename TFdBaseSock, typename TThread>
ESRV_RETCODE SocketServer<TImpl, TFdBaseSock, TThread>::InitServer()
{
    Release(true);

    ESRV_RETCODE rc = ESRV_RETCODE::SUCCESS;
    _fdEpoll.Fd(epoll_create1(0));
    if (false  == _fdEpoll.IsValid())
        rc = ESRV_RETCODE::ERROR_EPOLL;
    if (ESRV_RETCODE::SUCCESS == rc)
        rc = SetListener();
    return rc;
}


template <typename TImpl, typename TFdBaseSock, typename TThread>
ESRV_RETCODE SocketServer<TImpl, TFdBaseSock, TThread>::ConnectClient()
{
    return _fdSock.Connect() ? ESRV_RETCODE::SUCCESS : ESRV_RETCODE::ERROR_CONNECT;
}


template <typename TImpl, typename TFdBaseSock, typename TThread>
ESRV_RETCODE SocketServer<TImpl, TFdBaseSock, TThread>::Reconnect()
{
    ESRV_RETCODE rc = ESRV_RETCODE::ERROR_CONNECT;
    while (ESRV_RETCODE::SUCCESS != (rc = ConnectClient()))
    {
        LOG_ERROR << "Unabel to connect" << std::endl;
        LOG_ERROR << "errno : " << ErrnoText(errno) << ", " << errno <<  std::endl;
        LOG_ERROR << "Retrying..." << std::endl;

        if (ESRV_RETCODE::SUCCESS != (rc = InitClient()))
        {
            LOG_ERROR << "Unabel to Init" << std::endl;
            LOG_ERROR << "errno : " << ErrnoText(errno) << ", " << errno <<  std::endl;
        }
        usleep(1000);
    }

    return rc;
}


// Form TThread
template <typename TImpl, typename TFdBaseSock, typename TThread>
void SocketServer<TImpl, TFdBaseSock, TThread>::Run()
{
    if (ESRV_RETCODE::SUCCESS != SocketServer<TImpl, TFdBaseSock, TThread>::Wait())
    {
        LOG_ERROR << "Unabel to Wait" << std::endl;
        LOG_ERROR << "errno : " << ErrnoText(errno) << ", " << errno <<  std::endl;
        //return CLI_RETCODE::ERROR_QUIT;
    }
}


template <typename TImpl, typename TFdBaseSock, typename TThread>
ESRV_RETCODE SocketServer<TImpl, TFdBaseSock, TThread>::Start()
{
    if (false == _fdEpoll.IsValid())
        return ESRV_RETCODE::ERROR_EPOLL;

    int count = 0;

    {
        TFdBaseSock *sock  = &_fdSock;//_listServerFd[idx]._pSock;
        if (sock)
        {
            if (true == sock->Listen())
            {
                epoll_event event {};
                event.events    = EPOLLIN;
                event.data.fd   = sock->Fd();
                if (-1 == epoll_ctl(_fdEpoll.Fd(), EPOLL_CTL_ADD, sock->Fd(), &event))
                {
                    LOG_ERROR << "Unable ADD fd " << std::endl;
                    LOG_ERROR << "errno : " << errno << ", " << ErrnoText(errno) << std::endl;
//                    continue;
                }
                else
                {
                    ++count;
                }
            }
        }
    }

    if (count < 1)
    {
        LOG_ERROR << "No Client to add listeners. Server not started" << std::endl;
        return ESRV_RETCODE::ERROR_EPOLL;
    }
    return  (0 == TThread::Create("SocketServer")) ? ESRV_RETCODE::SUCCESS : ESRV_RETCODE::ERROR_PTHREAD;
}


template <typename TImpl, typename TFdBaseSock, typename TThread>
void SocketServer<TImpl, TFdBaseSock, TThread>::Stop()
{
    _doExit = true;
    TThread::Join();
}


template <typename TImpl, typename TFdBaseSock, typename TThread>
ESRV_RETCODE SocketServer<TImpl, TFdBaseSock, TThread>::ClientAccept(TFdBaseSock *fdListen, TFdBaseSock &fd)
{
    if (nullptr == fdListen)
        return ESRV_RETCODE::ERROR_PARAMETER;

    return fdListen->Accept(fd) ? ESRV_RETCODE::SUCCESS : ESRV_RETCODE::ERROR_ACCEPT;
}


template <typename TImpl, typename TFdBaseSock, typename TThread>
ESRV_RETCODE SocketServer<TImpl, TFdBaseSock, TThread>::ClientAdd(const TFdBaseSock &fd)
{
    if (!fd.IsValid())
        return ESRV_RETCODE::ERROR_SOCKET;

    if (!_fdEpoll.IsValid())
        return ESRV_RETCODE::ERROR_EPOLL;

    if (_fdSock == fd)
        return ESRV_RETCODE::ERROR_EXIST;

    epoll_event event {};
    event.events = EPOLLIN /*|EPOLLRDHUP|EPOLLHUP*/;
    event.data.fd = fd.Fd();

    int ret = epoll_ctl(_fdEpoll.Fd(), EPOLL_CTL_ADD, fd.Fd(), &event);
    if (0 != ret) {
        return ESRV_RETCODE::ERROR_EPOLL;
    }

    return ESRV_RETCODE::SUCCESS;
}


template <typename TImpl, typename TFdBaseSock, typename TThread>
ESRV_RETCODE SocketServer<TImpl, TFdBaseSock, TThread>::ClientRemove(int fd)
{
    if (!_fdEpoll.IsValid())
        return ESRV_RETCODE::ERROR_EPOLL;

    ESRV_RETCODE ret = (0 == epoll_ctl(_fdEpoll.Fd(), EPOLL_CTL_DEL, fd, NULL)) ? ESRV_RETCODE::SUCCESS : ESRV_RETCODE::ERROR_EPOLL;
    ::close(fd);

    return ret;
}


template <typename TImpl, typename TFdBaseSock, typename TThread>
ESRV_RETCODE SocketServer<TImpl, TFdBaseSock, TThread>::Wait()
{
    LOG_INFO << "Entering SocketServer Recv loop" << std::endl;
    constexpr int maxevents = 10;
    epoll_event events[maxevents] {};

    while (false == _doExit)
    {
        sigset_t smask;
        sigemptyset(&smask);
        sigaddset(&smask, SIGINT);

        timespec timeout {100, 1000};
        int len = epoll_pwait2(_fdEpoll.Fd(), events, maxevents, &timeout/*-1*/, &smask);

        if (len == 0)
        {
            thread_local int cnt = 0;
            if (0 == (cnt++ % 10))
                LOG_DEBUG << "Timed Out : epoll_wait, errno:" << ErrnoText(errno) << std::endl;
            continue;//return ESRV_RETCODE::ERROR_EPOLLWAIT;
        }
        else if (len == -1)
        {
            LOG_ERROR << "ERROR: epoll_wait, errno : ("<< errno << ") " << ErrnoText(errno) << std::endl;
            continue;//return ESRV_RETCODE::ERROR_EPOLLWAIT;
        }


        for (int i = 0; i < len; ++i)
        {
//            LOG_DEBUG <<  "epoll_event : \n" << ::to_string(events[i]) << std::endl;

            TFdBaseSock *sock = &_fdSock;
            if (sock && (events[i].data.fd == sock->Fd()))
            {
                TFdBaseSock fd;
                ESRV_RETCODE rc = ClientAccept(sock, fd);
                if (ESRV_RETCODE::SUCCESS != rc) {
                    LOG_ERROR << "ClientAccept - errno : " << ErrnoText(errno) << ", " << errno <<  std::endl;
                } else {
                    rc = ClientAdd(fd);
                    switch (rc) {
                        case ESRV_RETCODE::SUCCESS :
                            LOG_INFO << "Client Connected" << std::endl;
                            break;
                        case ESRV_RETCODE::ERROR_EXIST :
                            break;
                        default:
                            LOG_ERROR << "errno : " << ErrnoText(errno) << ", " << errno <<  std::endl;
                            break;
                    }
                }
            }

            if(events[i].events & EPOLLIN)
            {
                Payload packet;
                ESRV_RETCODE err = Recv(packet, events[i].data.fd);
//                LOG_DEBUG << "SocketServer::Wait - Recv : (" << to_string(err) << ", " << ErrnoText(errno) << ") " << std::endl;
//                LOG_DEBUG << to_string(packet)  << std::endl;


                switch(err)
                {
                    case ESRV_RETCODE::ERROR_CONNECT:
                        ClientRemove({events[i].data.fd});
                        LOG_INFO << "Client Disconnecting : " << ::to_string(err) << std::endl;
                        break;

                    case ESRV_RETCODE::ERROR_RECV:
                        ClientRemove({events[i].data.fd});
                        LOG_INFO << "Client Disconnecting : " << ::to_string(err) << std::endl;
                        break;

                    case ESRV_RETCODE::ERROR_NOT_CON:
                        break;

                    default:
                        ;//(static_cast<TPacketImpl*>(this))->Payload(/*buff*/packet.data(), bytes_received);
                }
            }
            else if((events[i].events & EPOLLRDHUP) || (events[i].events & EPOLLHUP))
            {
                ClientRemove({events[i].data.fd});
                LOG_INFO << "Client Disconnecting : " << std::endl;
            }
        }
    }

    LOG_INFO << "Leaving SocketServer Recv loop" << std::endl;
    return ESRV_RETCODE::SUCCESS;
}


template <typename TImpl, typename TFdBaseSock, typename TThread>
ESRV_RETCODE SocketServer<TImpl, TFdBaseSock, TThread>::Send(TFdBaseSock *client, uint8_t *payload, uint16_t lenPayload)
{
    uint8_t packet[_lenMaxPacket] {};
    ssize_t lenPacket = PacketCreate(packet, _lenMaxPacket, payload, lenPayload);
    if (lenPacket < 1)
        return ESRV_RETCODE::ERROR_PACKET;

    return client->Send(packet, lenPacket) ? ESRV_RETCODE::SUCCESS : ESRV_RETCODE::ERROR_SEND;
}


template <typename TImpl, typename TFdBaseSock, typename TThread>
ESRV_RETCODE SocketServer<TImpl, TFdBaseSock, TThread>::Recv(Payload &packet, int fd)
{
    TFdBaseSock fdObj;
    fdObj.Fd(fd);

    constexpr size_t lenBuff = 20*1012;
    uint8_t buff[lenBuff];
    ssize_t lenRecv = fdObj.TFdBaseSock::Recv(buff, lenBuff);

    if (-1 == lenRecv) {
        if (ENOTCONN == errno ) {
            return ESRV_RETCODE::ERROR_NOT_CON;
        } else {
            return ESRV_RETCODE::ERROR_RECV;
        }
    }
    if (0 == lenRecv)
        return ESRV_RETCODE::ERROR_CONNECT;

    for  (ssize_t pos = 0, len = 0; pos < lenRecv; pos += (len + 1))
    {
        len = PacketParse(packet, buff + pos, lenRecv - pos);
        if ((len == 0) || (len < 0)) {
            return ESRV_RETCODE::ERROR_RECV;
        }
        else {
            CRTPBase<TImpl>::Impl().OnPayload(fdObj, packet);
        }
    }

    return ESRV_RETCODE::SUCCESS;
}


template <typename TImpl, typename TFdBaseSock, typename TThread>
ssize_t SocketServer<TImpl, TFdBaseSock, TThread>::PacketParse(Payload &packet, const uint8_t buff[], const ssize_t lenBuff)
{
    enum class PARSE_STATE
    {
        BEGIN,
        MID, PLEN1, PLEN2, PAYLOAD,
        CRC1, CRC2, CRC3, CRC4,
    } state = PARSE_STATE::BEGIN;

    const uint8_t *pMID = _mid;

    uint16_t idxPayload = 0U;
    uint16_t idxMdi     = 0U;
    uint32_t crc        = 0U;

    for (ssize_t idx = 0; idx < lenBuff; ++idx)
    {
        switch(state)
        {
            case PARSE_STATE::BEGIN :
                state   = PARSE_STATE::MID;
                idxMdi  = idx;
                pMID    = _mid;
            [[fallthrough]];

            case PARSE_STATE::MID :
                if (buff[idx] == *pMID++)
                {
                    if ((pMID - _mid) >= (int64_t)_lenMid)
                    {
                        state = PARSE_STATE::PLEN1;
                    }
                }
                else
                {
                    state = PARSE_STATE::BEGIN;
                    idx = idxMdi;
                }
                break;


            case PARSE_STATE::PLEN1 :
                packet._len = (uint16_t)buff[idx];
                state = PARSE_STATE::PLEN2;
                break;

            case PARSE_STATE::PLEN2 :
                packet._len |= ((uint16_t)buff[idx]) << 8;
                if (Payload::_lenMaxPayload > packet._len)
                {
                    state = PARSE_STATE::PAYLOAD;
                }
                else
                {
                    //LOG_ERROR << "Payload lenght overflow." << std::endl;
                    state   = PARSE_STATE::BEGIN;
                    idx     = idxMdi;
                }
                break;

            case PARSE_STATE::PAYLOAD :
                packet._packet[idxPayload++] = buff[idx];
                if (idxPayload >= packet._len)
                {
                    idxPayload = 0;
                    state = PARSE_STATE::CRC1;
                }
                break;

            case PARSE_STATE::CRC1:
                crc = (uint32_t)buff[idx];
                state = PARSE_STATE::CRC2;
                break;

            case PARSE_STATE::CRC2:
                crc |= (((uint32_t)buff[idx]) << 8);
                state = PARSE_STATE::CRC3;
                break;

            case PARSE_STATE::CRC3:
                crc |= (((uint32_t)buff[idx]) << 16);
                state = PARSE_STATE::CRC4;
                break;

            case PARSE_STATE::CRC4:
                crc |= (((uint32_t)buff[idx]) << 24);
//                state = PARSE_STATE::BEGIN;

//                {
//                    LOG_DEBUG   << "Payload : "                     << std::endl;
//                    LOG_DEBUG   << to_string(packet)                << std::endl;
//                    LOG_DEBUG   << "crc     : " << crc              << std::endl;
//                    LOG_DEBUG   << "crcCalc : " << crcCalc          << std::endl;
//                }

                uint32_t crcCalc = crc32(111, packet._packet, packet._len);
                if (crcCalc == crc)
                {
                    return idx;
                }
                else
                {
                    LOG_DEBUG << "ERROR : CRC mismatch, packet discarded." << std::endl;
                    state = PARSE_STATE::BEGIN;
                    idx = idxMdi;
                }
                break;
        }
    }

    return -1;
}


template <typename TImpl, typename TFdBaseSock, typename TThread>
ssize_t  SocketServer<TImpl, TFdBaseSock, TThread>::PacketCreate(uint8_t packet[], const ssize_t lenPacket, const uint8_t payload[], const uint16_t lenPayload)
{
    if (lenPacket > (ssize_t)_lenMaxPacket)
        return -1;
    if (lenPayload > (ssize_t)Payload::_lenMaxPayload)
        return -1;

    //TODO: Add check -> (lenPacket > lenPayload + MidLen + PyLdLen + CrcLen)
    uint8_t *p = packet;

    const uint8_t *pMID = _mid;
    *p++ = *pMID++;
    *p++ = *pMID++;
    *p++ = *pMID++;
    *p++ = *pMID++;
    *p++ = (uint8_t)(lenPayload & 0xFF);
    *p++ = (uint8_t)((lenPayload >> 8) & 0xFF);

    std::memcpy(p, payload, lenPayload);
    p += lenPayload;

    uint32_t crc = crc32(111, (uint8_t *)payload, lenPayload);
    *p++ = (uint8_t) (crc & 0xFF);
    *p++ = (uint8_t)((crc >> 8) & 0xFF);
    *p++ = (uint8_t)((crc >> 16) & 0xFF);
    *p++ = (uint8_t)((crc >> 24) & 0xFF);

    ssize_t len = p - packet;
    return (len > lenPacket) ? -1 : len;
}



#endif // __SOCKET_SERVER_H__

