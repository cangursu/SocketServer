
#ifndef __SOCKET_SERVER_H__
#define __SOCKET_SERVER_H__

#include <iostream>
#include <sstream>
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


#define LOG_DEBUG std::cout
#define LOG_ERROR (std::cout << "ERROR : ")
#define LOG_INFO  std::cout



enum class ESRV_RETCODE
{
    NA              =   1,
    SUCCESS         =   0,
    ERROR_SOCKET    = - 1,
    ERROR_BIND      = - 2,
    ERROR_LISTEN    = - 3,
    ERROR_ACCEPT    = - 4,
    ERROR_CONNECT   = - 5,
    ERROR_SEND      = - 6,
    ERROR_RECV      = - 7,
    ERROR_PACKET    = - 8,
    ERROR_PARAMETER = - 9,
    ERROR_PTHREAD   = -40,
    ERROR_EPOLL     = -50,
    ERROR_EPOLLWAIT = -51,
};



const char *ErrnoText(int eno);
std::string to_string(const epoll_event &val);

void        to_string(ESRV_RETCODE val, std::ostringstream &ss);
std::string to_string(ESRV_RETCODE val);



struct Payload
{
    static constexpr uint16_t   _lenMaxPayload  = 256;//_lenMaxPacket - 10;

    uint8_t _packet [Payload::_lenMaxPayload];
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

template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount = 1>
class SocketServer
    : public TThread
{
    private :
        struct ClientPtr
        {
            TFdClient   * _pClient = nullptr;
            TFdBaseSock * _pSock   = nullptr;
        };

    public:
        static constexpr uint16_t   _lenMaxPacket   = 265;
        static constexpr uint8_t    _mid[]          = "AIZ:";
        static constexpr uint16_t   _lenMid         = 4;


        SocketServer()                   = default;
        SocketServer(const SocketServer&) = delete;
        SocketServer(SocketServer&&)      = delete;
        SocketServer &operator = (const SocketServer&)    = delete;
        SocketServer &operator = (SocketServer&&)         = delete;


        ESRV_RETCODE   InitClient(TFdClient *client, TFdBaseSock *sock);
        ESRV_RETCODE   InitServer(/*const std::string &key*/);
        void            Release(bool doUnlink = false);

        ESRV_RETCODE    SetListener(size_t idx, TFdClient *client, TFdBaseSock *sock);

        // Form TThread
        virtual void Run() override;

        ESRV_RETCODE    ConnectClient();
        ESRV_RETCODE    Reconnect(TFdClient *pClient, TFdBaseSock *pSock);
        ESRV_RETCODE    Recv(Payload &packet, int fd);
        ESRV_RETCODE    Send(TFdClient *client, uint8_t *payload, uint16_t lenPayload);
        ESRV_RETCODE    Start();
        void            Stop();
        ESRV_RETCODE    Wait();
        ESRV_RETCODE    ClientAccept(TFdBaseSock *fdListen, TFdBaseSock &fd);
        ESRV_RETCODE    ClientAdd(ClientPtr *ClientPtr, const TFdBaseSock &fd);
        ESRV_RETCODE    ClientRemove(int fd);

//    private :

        static ssize_t  PacketParse(Payload &packet, const uint8_t buff[], const ssize_t len);
        static ssize_t  PacketCreate(uint8_t packet[], const ssize_t lenPacket, const uint8_t payload[], const uint16_t lenPayload);

    private :

        TFdBaseSock                             _fdEpoll;
        std::array<ClientPtr, KClientCount>     _listClient {};
        std::map<int, ClientPtr*>               _mapClientFd;

        std::atomic<bool>                       _doExit     = false;
};


template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
void SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::Release(bool doUnlink /*= false*/)
{
    _fdEpoll.Close();
    for (auto &client : _listClient)
    {
        if (client._pSock)
        {
            client._pSock->Close();
            if (doUnlink)
                client._pSock->DeleteKey();
                //unlink(client._pSock->_key.data());
        }
    }
}


template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
ESRV_RETCODE SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::SetListener(size_t idx, TFdClient *client, TFdBaseSock *sock)
{
    if (nullptr == sock)
        return ESRV_RETCODE::ERROR_PARAMETER;
    if (nullptr == client)
        return ESRV_RETCODE::ERROR_PARAMETER;

//    if (sock->_key.empty())
//        return ESRV_RETCODE::ERROR_PARAMETER;
    if (idx > KClientCount)
        return ESRV_RETCODE::ERROR_PARAMETER;

    ClientPtr &ptr = _listClient[idx];
    ptr._pClient = client;
    ptr._pSock   = sock;

    return sock->Init() ? ESRV_RETCODE::SUCCESS : ESRV_RETCODE::ERROR_SOCKET;
}


template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
ESRV_RETCODE SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::InitClient(TFdClient *client, TFdBaseSock *sock)
{
    Release(false);
    return SetListener(0, client, sock);
}


template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
ESRV_RETCODE SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::InitServer(/*const std::string &key*/)
{
    Release(true);

    ESRV_RETCODE rc = ESRV_RETCODE::SUCCESS;
    _fdEpoll.Fd(epoll_create1(0));
    if (false  == _fdEpoll.IsValid())
        rc = ESRV_RETCODE::ERROR_EPOLL;
    return rc;
}


template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
ESRV_RETCODE SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::ConnectClient()
{
    ClientPtr   client = _listClient[0];
    TFdBaseSock *sock   = client._pSock;

    if (nullptr == sock)
        return ESRV_RETCODE::ERROR_PARAMETER;

    return sock->Connect() ? ESRV_RETCODE::SUCCESS : ESRV_RETCODE::ERROR_CONNECT;
}


template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
ESRV_RETCODE SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::Reconnect(TFdClient *pClient, TFdBaseSock *pSock)
{
    ESRV_RETCODE rc = ESRV_RETCODE::ERROR_CONNECT;
    while (ESRV_RETCODE::SUCCESS != (rc = ConnectClient()))
    {
        LOG_ERROR << "Unabel to connect" << std::endl;
        LOG_ERROR << "errno : " << ErrnoText(errno) << ", " << errno <<  std::endl;
        LOG_ERROR << "Retrying..." << std::endl;

        if (ESRV_RETCODE::SUCCESS != (rc = InitClient(pClient, pSock)))
        {
            LOG_ERROR << "Unabel to Init" << std::endl;
            LOG_ERROR << "errno : " << ErrnoText(errno) << ", " << errno <<  std::endl;
        }
        usleep(1000);
    }

    return rc;
}


// Form TThread
template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
void SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::Run()
{
    if (ESRV_RETCODE::SUCCESS != SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::Wait())
    {
        LOG_ERROR << "Unabel to Wait" << std::endl;
        LOG_ERROR << "errno : " << ErrnoText(errno) << ", " << errno <<  std::endl;
        //return CLI_RETCODE::ERROR_QUIT;
    }
}


template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
ESRV_RETCODE SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::Start()
{
    if (false == _fdEpoll.IsValid())
        return ESRV_RETCODE::ERROR_EPOLL;

    int count = 0;
    for (size_t idx = 0; idx < KClientCount; ++idx)
    {
        TFdBaseSock *sock  = _listClient[idx]._pSock;
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
                    continue;
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


template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
void SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::Stop()
{
    _doExit = true;
    TThread::Join();
}


template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
ESRV_RETCODE SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::ClientAccept(TFdBaseSock *fdListen, TFdBaseSock &fd)
{
    if (nullptr == fdListen)
        return ESRV_RETCODE::ERROR_PARAMETER;

    return fdListen->Accept(fd) ? ESRV_RETCODE::SUCCESS : ESRV_RETCODE::ERROR_ACCEPT;
}


template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
ESRV_RETCODE SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::ClientAdd(ClientPtr *client, const TFdBaseSock &fd)
{
    if (!fd.IsValid())
        return ESRV_RETCODE::ERROR_SOCKET;

    if (!_fdEpoll.IsValid())
        return ESRV_RETCODE::ERROR_EPOLL;

    epoll_event event {};
    event.events = EPOLLIN /*|EPOLLRDHUP|EPOLLHUP*/;
    event.data.fd = fd.Fd();
    int ret = epoll_ctl(_fdEpoll.Fd(), EPOLL_CTL_ADD, fd.Fd(), &event);
    if (0 != ret)
        return ESRV_RETCODE::ERROR_EPOLL;

    _mapClientFd[fd.Fd()] = client;
    return ESRV_RETCODE::SUCCESS;
}


template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
ESRV_RETCODE SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::ClientRemove(int fd)
{
    if (!_fdEpoll.IsValid())
        return ESRV_RETCODE::ERROR_EPOLL;

    ESRV_RETCODE ret = (0 == epoll_ctl(_fdEpoll.Fd(), EPOLL_CTL_DEL, fd, NULL)) ? ESRV_RETCODE::SUCCESS : ESRV_RETCODE::ERROR_EPOLL;
    ::close(fd);
    _mapClientFd.erase(fd);
    return ret;
}


template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
ESRV_RETCODE SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::Wait()
{
    LOG_INFO << "Entering SocketServer Recv loop" << std::endl;
    constexpr int maxevents = 128;
    epoll_event events[maxevents] {};

    while (false == _doExit)
    {
        sigset_t smask;
        sigemptyset(&smask);
        sigaddset(&smask, SIGINT);

        timespec timeout {1, 1000};
        int len = epoll_pwait2(_fdEpoll.Fd(), events, maxevents, &timeout/*-1*/, &smask);

        if (len == 0)
        {
//            thread_local int cnt = 0;
//            if (0 == (cnt++ % 10))
//                LOG_DEBUG << "Timed Out : epoll_wait, errno:" << ErrnoText(errno) << std::endl;
            continue;//return ESRV_RETCODE::ERROR_EPOLLWAIT;
        }
        else if (len == -1)
        {
            LOG_ERROR << "ERROR: epoll_wait, errno : " << ErrnoText(errno) << std::endl;
            continue;//return ESRV_RETCODE::ERROR_EPOLLWAIT;
        }


        for (int i = 0; i < len; ++i)
        {
//            LOG_DEBUG <<  "epoll_event : " << ::to_string(events[i]) << std::endl;

            bool isAccepted = false;
            for (auto &client : _listClient)
            {
                if (client._pSock && (events[i].data.fd == client._pSock->Fd()))
                {
                    TFdBaseSock fd;
                    if (ESRV_RETCODE::SUCCESS != ClientAccept(client._pSock, fd))
                    {
                        LOG_ERROR << "errno : " << ErrnoText(errno) << ", " << errno <<  std::endl;
                        continue;
                    }

                    if (ESRV_RETCODE::SUCCESS != ClientAdd(&client, fd))
                    {
                        LOG_ERROR << "errno : " << ErrnoText(errno) << ", " << errno <<  std::endl;
                        continue;
                    }

                    isAccepted = true;
                    LOG_INFO << "Client Connected" << std::endl;
                    continue;
                }
            }
            if (isAccepted)
                continue;

            if(events[i].events & EPOLLIN)
            {
                Payload packet;
                ESRV_RETCODE err = Recv(packet, events[i].data.fd);
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


template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
ESRV_RETCODE SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::Send(TFdClient *client, uint8_t *payload, uint16_t lenPayload)
{
    uint8_t packet[_lenMaxPacket] {};
    ssize_t lenPacket = PacketCreate(packet, _lenMaxPacket, payload, lenPayload);
    if (lenPacket < 1)
        return ESRV_RETCODE::ERROR_PACKET;

    return client->TFdBaseSock::Send(packet, lenPacket) ? ESRV_RETCODE::SUCCESS : ESRV_RETCODE::ERROR_SEND;
}


template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
ESRV_RETCODE SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::Recv(Payload &packet, int fd)
{
    TFdBaseSock fdObj(fd);

    constexpr size_t lenBuff = 20*1012;
    uint8_t buff[lenBuff];
    ssize_t lenRecv = fdObj.TFdBaseSock::Recv(buff, lenBuff);

    if (-1 == lenRecv)
        return ESRV_RETCODE::ERROR_RECV;
    if (0 == lenRecv)
        return ESRV_RETCODE::ERROR_CONNECT;

    for  (ssize_t pos = 0, len = 0; pos < lenRecv; pos += (len + 1))
    {
        len = PacketParse(packet, buff + pos, lenRecv - pos);
        if ((len == 0) || (len < 0))
            return ESRV_RETCODE::ERROR_RECV;
        else
        {
            auto it = _mapClientFd.find(fd);
            if (it != _mapClientFd.end() && it->second && it->second->_pClient)
                it->second->_pClient->Payload(packet);
        }
    }

    return ESRV_RETCODE::SUCCESS;
}


template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
ssize_t SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::PacketParse(Payload &packet, const uint8_t buff[], const ssize_t lenBuff)
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


template <typename TFdClient, typename TFdBaseSock, typename TThread, size_t KClientCount>
ssize_t  SocketServer<TFdClient, TFdBaseSock, TThread, KClientCount>::PacketCreate(uint8_t packet[], const ssize_t lenPacket, const uint8_t payload[], const uint16_t lenPayload)
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

