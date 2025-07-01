
cmake_minimum_required(VERSION 3.12)

project(SocketServerApp)

include_directories(inc)

file(GLOB_RECURSE HEADER "inc/*.hpp" "inc/*.h")
file(GLOB_RECURSE SOURCE "src/*.cpp" "src/*.c")

set (EVSRV_LIB           "${CMAKE_SOURCE_DIR}/lib/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS} -g3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS} -O3")


# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(SocketServerUDS src/main.cpp ${HEADER})
target_compile_definitions(SocketServerUDS PUBLIC FDBASE_UDS)

add_executable(SocketServerTCP src/main.cpp ${HEADER})
target_compile_definitions(SocketServerTCP PUBLIC FDBASE_TCP)

add_executable(SocketServerUDP src/main.cpp ${HEADER})
target_compile_definitions(SocketServerUDP PUBLIC FDBASE_UDP)

#      add_executable(SocketServerUDS src/mainUds.cpp src/cli.cpp inc/cli.h)
#      #set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME SocketServer)
#      add_executable(SocketServerTCP src/mainTcp.cpp src/cli.cpp inc/cli.h)

# link libraries
target_include_directories(SocketServerUDS
            PRIVATE  "inc"
            PRIVATE   "${EVSRV_LIB}/inc" )
target_include_directories(SocketServerTCP
            PRIVATE  "inc"
            PRIVATE   "${EVSRV_LIB}/inc" )
target_include_directories(SocketServerUDP
            PRIVATE  "inc"
            PRIVATE   "${EVSRV_LIB}/inc" )

# target_link_libraries(${PROJECT_NAME} -lpthread -lboost_system -lssl -lcrypto -lrt)
target_link_libraries(SocketServerUDS SocketServer)
target_link_libraries(SocketServerTCP SocketServer)
target_link_libraries(SocketServerUDP SocketServer)
