
#include "SocketServer.hpp"
#include "FdBaseUds.hpp"
#include "FdBaseTcp.hpp"
#include "FdBaseUdp.hpp"
#include "PThread.hpp"
#include "cli.h"

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


#define DEFAULT_PORT 19000


class EchoServer
    : public SocketServer<EchoServer, FdBase, PThread>
{
    public :
        ~EchoServer()   { SocketServer<EchoServer, FdBase, PThread>::Release();                                    }

#if defined FDBASE_UDS
        EchoServer(const char *key = "/tmp/socket") : SocketServer<EchoServer, FdBase, PThread>(key) {}
#elif defined FDBASE_TCP
        EchoServer(uint16_t port = DEFAULT_PORT) : SocketServer<EchoServer, FdBase, PThread>("", port) {}
#elif defined FDBASE_UDP
        EchoServer(uint16_t port = DEFAULT_PORT) : SocketServer<EchoServer, FdBase, PThread>("", port) {}
#endif

        void            Release(bool doUnlink = false)  { return SocketServer<EchoServer, FdBase, PThread>::Release(doUnlink);  }
        ESRV_RETCODE    InitServer()                    { return SocketServer<EchoServer, FdBase, PThread>::InitServer();       }
        ESRV_RETCODE    Start()                         { return SocketServer<EchoServer, FdBase, PThread>::Start();            }
        void            Stop()                          { return SocketServer<EchoServer, FdBase, PThread>::Stop();             }

        void OnPayload(FdBase &client, /*const*/ ::Payload &pack) /*const*/;
};

void EchoServer::OnPayload(FdBase &client, /*const*/ ::Payload &packet) /*const*/
{
    LOG_INFO << "EchoServer::Payload Packet (" << client.Fd() << ", "
#if (defined FDBASE_TCP) || (defined FDBASE_UDP)
            << client.Addr().sin_port <<  ":" << client.Addr().sin_addr.s_addr
#elif defined FDBASE_UDS
            << client.Addr().sun_path
#endif //FDBASE_TCP, FDBASE_UDP
            << ") : " << to_string(packet)  << std::endl;

    SocketServer<EchoServer, FdBase, PThread>::Send(&client, packet._packet, packet._len);
}





CLI_RETCODE CmdExit(EchoServer &server, const char * /*cline*/)
{
    LOG_INFO << __func__ << std::endl;
    server.Stop();

    return CLI_RETCODE::ERROR_QUIT;
}


CLI_RETCODE CmdStart(EchoServer &server, const char * /*cline*/)
{
    if (ESRV_RETCODE::SUCCESS != server.Start())
    {
        LOG_INFO << "Unable to Start" << std::endl;
        LOG_INFO << "errno : " << ErrnoText(errno) << ", " << errno <<  std::endl;
        return CLI_RETCODE::ERROR_QUIT;
    }

    return CLI_RETCODE::SUCCESS;
}


CLI_RETCODE CmdStop(EchoServer &server, const char * /*cline*/)
{
    LOG_INFO << __func__ << std::endl;
    server.Stop();

    return CLI_RETCODE::SUCCESS;
}






int main(int argc, const char *argv[])
{
#if defined FDBASE_UDS

    LOG_INFO << "Hello Unix Domain Socket Server V1.0" << std::endl;
    const char *key = "/tmp/socket";
    if (argc > 1) {
        key = argv[1];
    }
    EchoServer server(key);


#elif defined FDBASE_TCP

    LOG_INFO << "Hello TCP Socket Server V1.0" << std::endl;
    uint16_t port = DEFAULT_PORT;

    if (argc > 1) {
        port = std::atoi(argv[1]);
        if (0 == port) {
            port = DEFAULT_PORT;
        }
    }
    EchoServer server(port);


#elif defined FDBASE_UDP

    LOG_INFO << "Hello UDP Socket Server V1.0" << std::endl;
    uint16_t    port = DEFAULT_PORT;
    if (argc > 1) {
        port = std::atoi(argv[1]);
        if (0 == port) {
            port = DEFAULT_PORT;
        }
    }
    EchoServer server(port);


#else
    LOG_INFO << "NO FdBase implemantaiton defined" << std::endl;
    return -1;
#endif



    CmdMap<EchoServer> mapCmd("mapCmd");
    mapCmd["exit"]  = { "Exit",         "Quits the program",    CmdExit     };
    mapCmd["quit"]  = { "Exit",         "Quits the program",    CmdExit     };
    mapCmd["q"]     = { "Exit",         "Quits the program",    CmdExit     };
    mapCmd["start"] = { "Start Server", "Starts the Server",    CmdStart    };
    mapCmd["s"]     = { "Start Server", "Starts the Server",    CmdStart    };
    mapCmd["stop"]  = { "Stop Server",  "Stops the Server",     CmdStop     };


    ESRV_RETCODE rc = ESRV_RETCODE::NA;
    if (ESRV_RETCODE::SUCCESS != (rc = server.InitServer()))
    {
        LOG_INFO << "Unabel to create Server" << std::endl;
        LOG_INFO << "rc : " << to_string(rc) << ", errno : " << ErrnoText(errno) << ", " << errno <<  std::endl;
        return -1;
    }

    CmdStart(server, "");

    auto fn = [&mapCmd, &server] (const char *cline) -> CLI_RETCODE
    {
        CLI_RETCODE rc = mapCmd.ExecuteCommand(server, cline);
        return rc;
    };

    cli(fn);

    server.Release(true);
    return 0;
}
