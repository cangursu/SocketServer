
#include "SocketServer.hpp"
#include "FdBaseUds.hpp"
#include "FdBaseTcp.hpp"
#include "PThread.hpp"

#include <iostream>


#if defined FDBASE_UDS
using FdBase = FdBaseUds;
#elif defined FDBASE_TCP
using FdBase = FdBaseTcp;
#else
#error "NO FdBase implemantaiton defined"
#endif


class SocketClientImpl
    : public FdBase
{
    public :
        ~SocketClientImpl()  = default;

#if defined FDBASE_UDS
        SocketClientImpl() : FdBase("/tmp/socket")                          {                                                       }
#elif defined FDBASE_TCP
        SocketClientImpl() : FdBase("127.0.0.1", 12123)                     {                                                       }
#endif

        void            Release(bool doUnlink = false)                  { return _client.Release(doUnlink);                     }
        ESRV_RETCODE    InitClient()                                    { return _client.InitClient(this, this);                }
        ESRV_RETCODE    Connect()                                       { return _client.ConnectClient();                       }
        ESRV_RETCODE    Reconnect()                                     { return _client.Reconnect(this, this);                 }
        ESRV_RETCODE    Send(uint8_t *payload, uint16_t len)            { return _client.Send(this, payload, len);              }

        void Payload(/*const*/ ::Payload &pack) /*const*/;

        SocketServer<SocketClientImpl, FdBase, PThread>   _client;
};

void SocketClientImpl::Payload(/*const*/ ::Payload &pack) /*const*/
{
    std::cout << __PRETTY_FUNCTION__  << " : " << std::endl;

    pack._packet[pack._len] = '\0'; // Null-terminate the string
    std::cout << "Received from client: " << (char*)pack._packet << std::endl;
}





int main(int /*argc*/, const char * /*argv*/[])
{
#if defined FDBASE_UDS
    std::cout << "Hello Unix Domain Socket Client V0.0 " << std::endl;
#elif defined FDBASE_TCP
    std::cout << "Hello TCP Socket Client V0.0 " << std::endl;
#else
    std::cout << "NO FdBase implemantaiton defined" << std::endl;
    return -1;
#endif



    ESRV_RETCODE rc = ESRV_RETCODE::NA;

    SocketClientImpl client;
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


    for (int i = 0; i < 20; ++i)
    {
        std::string message = "Hello, server ! ";
        message += std::to_string(i);
        size_t sz = message.size();

        LOG_DEBUG << "\n";
        LOG_DEBUG << "sz     :" << sz      << std::endl;
        LOG_DEBUG << "message:" << message << std::endl;

        ESRV_RETCODE rc = client.Send((uint8_t*)(message.data()), sz);
        LOG_DEBUG << "Send : " << to_string(rc) << std::endl;
        if (ESRV_RETCODE::SUCCESS != rc)
        {
            LOG_ERROR << "ERROR Send" << std::endl;
            client.Reconnect();
        }

        sleep(1);
    }

    client.Release();
    return 0;
}
