
#include "SocketServer.hpp"
#include "FdBaseUds.hpp"
#include "FdBaseTcp.hpp"
#include "FdBaseUdp.hpp"
#include "PThread.hpp"

#include <iostream>


#if defined FDBASE_UDS
using FdBase = FdBaseUds;
#elif defined FDBASE_TCP
using FdBase = FdBaseTcp;
#elif defined FDBASE_UDP
using FdBase = FdBaseUdp;
#else
#error "NO FdBase implemantaiton defined"
#endif


class EchoClient
    : public SocketServer<EchoClient, FdBase, PThread>
{
    public :
        ~EchoClient()  = default;

#if defined FDBASE_UDS

        EchoClient(const char *key)
            : SocketServer<EchoClient, FdBase, PThread>(key)
{
}
        EchoClient()
            : SocketServer<EchoClient, FdBase, PThread>("/tmp/socket")
{
}

#elif defined FDBASE_TCP

        EchoClient(const char *ip, uint16_t port)
            : SocketServer<EchoClient, FdBase, PThread>(ip, port)
{
}
        EchoClient()
            : SocketServer<EchoClient, FdBase, PThread>("127.0.0.1", 8888)
{
}

#elif defined FDBASE_UDP

        EchoClient(uint16_t port)
            : SocketServer<EchoClient, FdBase, PThread>(port)
{
}
        EchoClient()
            : SocketServer<EchoClient, FdBase, PThread>(8888)
{
}

#endif

        void            Release(bool doUnlink = false)          { return SocketServer<EchoClient, FdBase, PThread>::Release(doUnlink);              }
        ESRV_RETCODE    InitClient()                            { return SocketServer<EchoClient, FdBase, PThread>::InitClient();                   }
        ESRV_RETCODE    Connect()                               { return SocketServer<EchoClient, FdBase, PThread>::ConnectClient();                }
        ESRV_RETCODE    Reconnect()                             { return SocketServer<EchoClient, FdBase, PThread>::Reconnect();                    }
        ESRV_RETCODE    Send(uint8_t *payload, uint16_t len)    { return SocketServer<EchoClient, FdBase, PThread>::Send(&Sock() , payload, len);   }

        void OnPayload(FdBase &client, /*const*/ ::Payload &pack) /*const*/;
};

void EchoClient::OnPayload(FdBase &client, /*const*/ ::Payload &pack) /*const*/
{
//    LOG_INFO << __PRETTY_FUNCTION__  << " : " << std::endl;
    pack._packet[pack._len] = '\0'; // Null-terminate the string
    LOG_INFO << "Received from remote (" << client.Fd() << ") : " << (char*)pack._packet << std::endl;
}





int main(int /*argc*/, const char * /*argv*/[])
{
#if defined FDBASE_UDS
    std::cout << "Unix Domain Socket Client V0.0 " << std::endl;
#elif defined FDBASE_TCP
    std::cout << "Hello TCP Socket Client V0.0 " << std::endl;
#elif defined FDBASE_UDP
    std::cout << "Hello UDP Socket Client V0.0 " << std::endl;
#else
    std::cout << "NO FdBase implemantaiton defined" << std::endl;
    return -1;
#endif



    ESRV_RETCODE rc = ESRV_RETCODE::NA;

    EchoClient client;
    if (ESRV_RETCODE::SUCCESS != (rc = client.InitClient()))
    {
        LOG_ERROR << "Unable to init Client" << std::endl;
        return -1;
    }

    if (ESRV_RETCODE::SUCCESS != (rc = client.Connect()))
    {
        LOG_ERROR << "Unable to Connect" << std::endl;
        return -1;
    }


    constexpr int count = 20;
    for (int i = 0; i < count; ++i)
    {
        std::string message = "Hello, server ! ";
        message += std::to_string(i);
        size_t sz = message.size();

        //LOG_DEBUG << "\n";
        //LOG_DEBUG << "sz     :" << sz      << std::endl;
        //LOG_DEBUG << "message:" << message << std::endl;

        ESRV_RETCODE rc = client.Send((uint8_t*)(message.data()), sz);
//        LOG_DEBUG << "Send : " << message << " : " << to_string(rc) << std::endl;
        if (ESRV_RETCODE::SUCCESS != rc)
        {
            LOG_ERROR << "ERROR Send" << std::endl;
            client.Reconnect();
        }
        else {
            Payload packet;
            ESRV_RETCODE err = client.Recv(packet, client.Sock().Fd());
            LOG_DEBUG << "Recv : " << std::string(reinterpret_cast<const char*>(packet._packet), packet._len) << ", " << ::to_string(err) << ") : " << std::endl;
        }

        sleep(1);
    }

    client.Release();
    return 0;
}
