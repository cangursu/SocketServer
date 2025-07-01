
cmake_minimum_required(VERSION 3.12)

project(SocketClient)

include_directories(inc)

file(GLOB_RECURSE HEADER "inc/*.hpp" "inc/*.h")
file(GLOB_RECURSE SOURCE "src/*.cpp" "src/*.c")

set (EVSRV_LIB           "${CMAKE_SOURCE_DIR}/lib/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS} -g3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS} -O3")


# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(SocketClientUDS ${SOURCE} ${HEADER})
target_compile_definitions(SocketClientUDS PUBLIC FDBASE_UDS)

add_executable(SocketClientTCP ${SOURCE} ${HEADER})
target_compile_definitions(SocketClientTCP PUBLIC FDBASE_TCP)

add_executable(SocketClientUDP ${SOURCE} ${HEADER})
target_compile_definitions(SocketClientUDP PUBLIC FDBASE_UDP)

# source_group("Header include" FILES ${HEADER})
# source_group("Source src"     FILES ${SOURCE})

# link libraries
target_include_directories(SocketClientUDS
            PRIVATE  "inc"
            PRIVATE   "${EVSRV_LIB}/inc" )
target_include_directories(SocketClientTCP
            PRIVATE  "inc"
            PRIVATE   "${EVSRV_LIB}/inc" )
target_include_directories(SocketClientUDP
            PRIVATE  "inc"
            PRIVATE   "${EVSRV_LIB}/inc" )

# target_link_libraries(${PROJECT_NAME} -lpthread -lboost_system -lssl -lcrypto -lrt)
target_link_libraries(SocketClientUDS SocketServer)
target_link_libraries(SocketClientTCP SocketServer)
target_link_libraries(SocketClientUDP SocketServer)
