
cmake_minimum_required(VERSION 3.12)

project(SocketServer_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

file(GLOB_RECURSE HEADER_TEST "inc/*.hpp" "inc/*.h")
file(GLOB_RECURSE SOURCE_TEST "src/*.cpp" "src/*.c")

list (APPEND HEADER ${HEADER_TEST})
list (APPEND SOURCE ${SOURCE_TEST})

file(GLOB_RECURSE HEADER_LIB "${CMAKE_SOURCE_DIR}/lib/inc/*.hpp" "${CMAKE_SOURCE_DIR}/lib/inc/*.h" )   #recompile lib
file(GLOB_RECURSE SOURCE_LIB "${CMAKE_SOURCE_DIR}/lib/src/*.cpp" "${CMAKE_SOURCE_DIR}/lib/src/*.c" )   #recompile lib

list (APPEND HEADER ${HEADER_LIB})
list (APPEND SOURCE ${SOURCE_LIB})

#file(GLOB_RECURSE HEADER_SRC "${CMAKE_SOURCE_DIR}/client/inc/*.hpp" "${CMAKE_SOURCE_DIR}/client/inc/*.h"
#                             "${CMAKE_SOURCE_DIR}/server/inc/*.hpp" "${CMAKE_SOURCE_DIR}/server/inc/*.h")   #recompile src
#file(GLOB_RECURSE SOURCE_SRC "${CMAKE_SOURCE_DIR}/client/src/*.cpp" "${CMAKE_SOURCE_DIR}/client/src/*.c"
#                             "${CMAKE_SOURCE_DIR}/server/src/*.cpp" "${CMAKE_SOURCE_DIR}/server/src/*.c")   #recompile src

list (APPEND HEADER ${HEADER_SRC})
list (APPEND SOURCE ${SOURCE_SRC})

foreach(item ${SOURCE})
    if(${item} MATCHES "server/src/main.cpp")
        list(REMOVE_ITEM SOURCE ${item})
    endif(${item} MATCHES "server/src/main.cpp")
    if(${item} MATCHES "client/src/main.cpp")
        list(REMOVE_ITEM SOURCE ${item})
    endif(${item} MATCHES "client/src/main.cpp")
endforeach(item)


find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME}  ${SOURCE} ${HEADER})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/lib/inc" "inc" )
target_link_libraries(${PROJECT_NAME} PRIVATE -lz -lreadline GTest::GTest)



