cmake_minimum_required(VERSION 3.6)
project(os_lab3)

set(CMAKE_CXX_STANDARD 11)


set(SRC_LIST
        src/Servers.h
        src/Servers.cpp)

add_executable(server src/server.cpp ${SRC_LIST})
add_executable(client src/client.cpp ${SRC_LIST} src/client.cpp src/Servers.h src/utils.hpp src/Servers.cpp)
