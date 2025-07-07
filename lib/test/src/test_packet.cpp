
#include "SocketServer.hpp"
#include "FdBaseUds.hpp"

#include <gtest/gtest.h>


class UdsTest
{
    public :
        void Payload(::Payload &) {}
};



TEST(SocketServerPacket, ParseSimple1)
{
    constexpr ssize_t lenPacketBuff = 128;
    uint8_t packet[lenPacketBuff];

    char pload[] = "TestPayload";
    ssize_t lenpload = std::strlen((char*)pload);

    size_t      lenPacket = SocketServer<UdsTest, FdBaseUds>::PacketCreate(packet, lenPacketBuff, (uint8_t *)pload, lenpload);

    size_t      lenMid          = SocketServer<UdsTest, FdBaseUds>::_lenMid;
    uint16_t    lenPayloadLen   = 2;
    uint16_t    lenCrc          = 4;
    EXPECT_EQ ( lenpload + lenMid + lenPayloadLen + lenCrc, lenPacket);

    ::Payload pck;
    size_t      lenPck = SocketServer<UdsTest, FdBaseUds>::PacketParse(pck, packet, lenPacket);
    EXPECT_EQ (lenPck + 1, lenPacket);
    EXPECT_EQ (0, std::memcmp(pload, pck._packet, pck._len));
}


TEST(SocketServerPacket, ParseSimple2)
{
    constexpr ssize_t lenPacketBuff = 128;
    uint8_t packet[lenPacketBuff];
    for (size_t idx = 0; idx < lenPacketBuff; ++idx)
        packet[idx] = 0xFE;


    char pload[] = "TestPayload";
    ssize_t lenpload = std::strlen((char*)pload);

    size_t  lenPacketDirt = 0U;
    packet[lenPacketDirt++] = 'A';
    packet[lenPacketDirt++] = 'I';
    packet[lenPacketDirt++] = 'Z';
    packet[lenPacketDirt++] = ':';
    packet[lenPacketDirt++] = 'A';
    packet[lenPacketDirt++] = 'A';
    packet[lenPacketDirt++] = 'I';
    packet[lenPacketDirt++] = 'A';
    packet[lenPacketDirt++] = 'Z';
    packet[lenPacketDirt++] = 'A';
    packet[lenPacketDirt++] = 'A';
    packet[lenPacketDirt++] = 'A';
    packet[lenPacketDirt++] = 'A';
    packet[lenPacketDirt++] = 'I';
    packet[lenPacketDirt++] = 'Z';
    packet[lenPacketDirt++] = ':';
    size_t lenPacket = SocketServer<UdsTest, FdBaseUds>::PacketCreate(packet + lenPacketDirt, lenPacketBuff, (uint8_t *)pload, lenpload);

    size_t      lenMid          = SocketServer<UdsTest, FdBaseUds>::_lenMid;
    uint16_t    lenPayloadLen   = 2;
    uint16_t    lenCrc          = 4;
    EXPECT_EQ ( lenpload + lenMid + lenPayloadLen + lenCrc, lenPacket);

    ::Payload pck;
    size_t      lenPck = SocketServer<UdsTest, FdBaseUds>::PacketParse(pck, packet, lenPacket + lenPacketDirt);
    EXPECT_EQ (lenpload, pck._len);
    EXPECT_EQ (lenPck + 1, lenPacket + lenPacketDirt);
    EXPECT_EQ (0, std::memcmp(pload, pck._packet, pck._len));
}
