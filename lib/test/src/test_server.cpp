
#include "SocketServer.hpp"
#include "FdBaseUds.hpp"
#include "PThread.hpp"

#include <gtest/gtest.h>
#include <string>



class UdsTestServer
    : public FdBaseUds
    , public SocketServer<UdsTestServer, FdBaseUds, PThread>
{
    public :
        UdsTestServer(const std::string &key)
            : FdBaseUds(key)
        {
        }

        ESRV_RETCODE InitClient()                       { return SocketServer<UdsTestServer, FdBaseUds, PThread>::InitClient(this, this);   }
        ESRV_RETCODE Send(uint8_t *data, uint16_t len)  { return SocketServer<UdsTestServer, FdBaseUds, PThread>::Send(this, data, len);    }

        void Payload(::Payload &);
        uint64_t _data = 0;
};


void UdsTestServer::Payload(::Payload &payload)
{
    EXPECT_EQ(sizeof(_data), payload._len);
    EXPECT_EQ(++_data, *((decltype(_data)*)payload._packet));
    //std::cout << "payload : " << _data << ", " << *(payload._packet) << std::endl;
}


TEST(SocketServerLib, Test)
{
    const std::string key = "./SockTest1";

    UdsTestServer server(key);
    EXPECT_EQ(ESRV_RETCODE::SUCCESS,    server.InitServer());
    EXPECT_EQ(ESRV_RETCODE::SUCCESS,    server.SetListener(0, &server, &server));
    EXPECT_EQ(ESRV_RETCODE::SUCCESS,    server.Start());

    UdsTestServer client(key);
    EXPECT_EQ(ESRV_RETCODE::SUCCESS,    client.InitClient());
    EXPECT_EQ(ESRV_RETCODE::SUCCESS,    client.ConnectClient());


    uint64_t count = 1000;
    for (uint64_t data = 1; data < count; ++data)
        client.Send(reinterpret_cast<uint8_t*>(&data), sizeof(uint64_t));

    sleep(1);
    server.Stop();

//    std::cout << "server._data : " << server._data << std::endl;

    EXPECT_EQ(count,  server._data + 1);
    client.Release();
    server.Release(true);
}





class UdsTestServer_Src1
    : public FdBaseUds
    , public PayloadImpl
{
    public :
        UdsTestServer_Src1(const std::string &key)
            : FdBaseUds(key)
        {
        }

        void Payload(::Payload &pck)
        {
            EXPECT_NE(-1, _data);
            EXPECT_EQ( 1, pck._len);
            _data = pck._packet[0];
        }
    uint8_t _data = 0xFF;
};


class UdsTestServer_Src2
    : public FdBaseUds
    , public PayloadImpl
{
    public :
        UdsTestServer_Src2(const std::string &key)
            : FdBaseUds(key)
        {
        }

        void Payload(::Payload &pck)
        {
            EXPECT_NE(-1, _data);
            EXPECT_EQ( 1, pck._len);
            _data = pck._packet[0];
        }
    uint8_t _data = 0xFF;
};


class UdsTestServer_MultiSource
    : public UdsTestServer_Src1
    , public UdsTestServer_Src2
{
    public :
        UdsTestServer_MultiSource(const std::string &key1, const std::string &key2)
            : UdsTestServer_Src1(key1)
            , UdsTestServer_Src2(key2)
        {
        }

    SocketServer<PayloadImpl, FdBaseUds, PThread, 2> _server;
};


TEST(SocketServerLib, TestMultiConnection)
{
    const std::string key1 = "./SockTestMS1";
    const std::string key2 = "./SockTestMS2";

    UdsTestServer_MultiSource srv(key1, key2);

    EXPECT_EQ(ESRV_RETCODE::SUCCESS,  srv._server.InitServer());

    EXPECT_EQ(ESRV_RETCODE::SUCCESS,  srv._server.SetListener(0,
                                dynamic_cast<UdsTestServer_Src1*>(&srv),
                                dynamic_cast<UdsTestServer_Src1*>(&srv)   )  );
    EXPECT_EQ(ESRV_RETCODE::SUCCESS,  srv._server.SetListener(1,
                                dynamic_cast<UdsTestServer_Src2*>(&srv),
                                dynamic_cast<UdsTestServer_Src2*>(&srv)   )  );
    EXPECT_EQ(ESRV_RETCODE::SUCCESS,  srv._server.Start());


    UdsTestServer client1(key1);
    EXPECT_EQ(ESRV_RETCODE::SUCCESS,  client1.InitClient());
    EXPECT_EQ(ESRV_RETCODE::SUCCESS,  client1.ConnectClient());

    UdsTestServer client2(key2);
    EXPECT_EQ(ESRV_RETCODE::SUCCESS,  client2.InitClient());
    EXPECT_EQ(ESRV_RETCODE::SUCCESS,  client2.ConnectClient());

    uint8_t data1 = 0xa7;
    uint8_t data2 = 0x29;

    client1.Send(&data1, sizeof(data1));
    client2.Send(&data2, sizeof(data2));

    sleep(1);

    EXPECT_EQ(data1, srv. UdsTestServer_Src1::_data);
    EXPECT_EQ(data2, srv. UdsTestServer_Src2::_data);



    srv._server.Stop();
    srv._server.Release(true);
    client1.Release();
    client2.Release();
}

