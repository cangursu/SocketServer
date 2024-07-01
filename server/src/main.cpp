
#include "SocketServer.hpp"
#include "FdBaseUds.hpp"
#include "FdBaseTcp.hpp"
#include "PThread.hpp"
#include "cli.h"

#include <iostream>


#if defined FDBASE_UDS
using FdBase = FdBaseUds;
#elif defined FDBASE_TCP
using FdBase = FdBaseTcp;
#else
#error "NO FdBase implemantaiton defined"
#endif


class SocketServerImpl
    : public FdBase
{
    public :
        ~SocketServerImpl()                                              { _server.Release();                                    }

#if defined FDBASE_UDS
        SocketServerImpl() : FdBase("/tmp/socket")                       {                                                       }
#elif defined FDBASE_TCP
        SocketServerImpl() : FdBase("127.0.0.1", 12123)                  {                                                       }
#endif


        void            Release(bool doUnlink = false)                  { return _server.Release(doUnlink);                     }
        ESRV_RETCODE    InitServer(/*const std::string &key*/)          { return _server.InitServer();                          }
        ESRV_RETCODE    Start()                                         { return _server.Start();                               }
        void            Stop()                                          { return _server.Stop();                                }
        ESRV_RETCODE    SetListener(size_t idx, SocketServerImpl *client){ return _server.SetListener(idx, client, client);     }


        void Payload(/*const*/ ::Payload &pack) /*const*/;

        SocketServer<SocketServerImpl, FdBase, PThread, 1> _server;
};

void SocketServerImpl::Payload(/*const*/ ::Payload &packet) /*const*/
{
    //std::cout   << "\n" << __PRETTY_FUNCTION__            << std::endl;
    //std::cout   << "SocketServerImpl::Payload Packet  : " << std::endl;
    std::cout   << to_string(packet)  << std::endl;
}





CLI_RETCODE CmdExit(SocketServerImpl &server, const char * /*cline*/)
{
    std::cout << __func__ << std::endl;
    server.Stop();

    return CLI_RETCODE::ERROR_QUIT;
}


CLI_RETCODE CmdStart(SocketServerImpl &server, const char * /*cline*/)
{
    if (ESRV_RETCODE::SUCCESS != server.Start())
    {
        std::cout << "Unable to Start" << std::endl;
        std::cout << "errno : " << ErrnoText(errno) << ", " << errno <<  std::endl;
        return CLI_RETCODE::ERROR_QUIT;
    }

    return CLI_RETCODE::SUCCESS;
}


CLI_RETCODE CmdStop(SocketServerImpl &server, const char * /*cline*/)
{
    std::cout << __func__ << std::endl;
    server.Stop();

    return CLI_RETCODE::SUCCESS;
}






int main(int /*argc*/, const char * /*argv*/[])
{
#if defined FDBASE_UDS
    std::cout << "Hello Unix Domain Socket Server V1.0" << std::endl;
#elif defined FDBASE_TCP
    std::cout << "Hello TCP Socket Server V1.0" << std::endl;
#else
    std::cout << "NO FdBase implemantaiton defined" << std::endl;
    return -1;
#endif



    CmdMap<SocketServerImpl> mapCmd("mapCmd");
    mapCmd["exit"]  = { "Exit",         "Quits the program",    CmdExit     };
    mapCmd["quit"]  = { "Exit",         "Quits the program",    CmdExit     };
    mapCmd["q"]     = { "Exit",         "Quits the program",    CmdExit     };
    mapCmd["start"] = { "Start Server", "Starts the Server",    CmdStart    };
    mapCmd["s"]     = { "Start Server", "Starts the Server",    CmdStart    };
    mapCmd["stop"]  = { "Stop Server",  "Stops the Server",     CmdStop     };


    ESRV_RETCODE rc = ESRV_RETCODE::NA;
    SocketServerImpl server;

    if (ESRV_RETCODE::SUCCESS != (rc = server.InitServer()))
    {
        std::cout << "Unabel to create Server" << std::endl;
        std::cout << "rc : " << to_string(rc) << ", errno : " << ErrnoText(errno) << ", " << errno <<  std::endl;
        return -1;
    }

    if (ESRV_RETCODE::SUCCESS != (rc = server.SetListener(0, &server)))
    {
        std::cout << "Unabel to add listener" << std::endl;
        std::cout << "rc : " << to_string(rc) << ", errno : " << ErrnoText(errno) << ", " << errno <<  std::endl;
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
