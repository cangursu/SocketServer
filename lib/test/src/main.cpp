
#include <gtest/gtest.h>



int main(int argc, char** argv)
{
    std::cout << "SocketServer Lib Test Suit V1.3" << std::endl;
    std::cout << "argc : " << argc << std::endl;
    for (int idx = 0; idx < argc; ++idx)
        std::cout << "argv[" << idx << "] = " << argv[idx] << std::endl;
    std::cout << std::endl;


//    ::testing::GTEST_FLAG(filter) = "SocketServerPacket.*";
//    ::testing::GTEST_FLAG(filter) = "SocketServerLib.Test1";
//    ::testing::GTEST_FLAG(filter) = "SocketServerLib.TestMultiConnection";


    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

