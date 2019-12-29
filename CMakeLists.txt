cmake_minimum_required(VERSION 3.12)
project(protolink)

set(CMAKE_C_STANDARD 11)

add_executable(protolink
        main.cpp
        protolink.h
        matrix.h matrix.cpp
        tcpwrapper.cpp tcpwrapper.h
        TCPServer.cpp TCPServer.h
        TCPSocket.cpp TCPSocket.h)

find_package(SFML 2.5 COMPONENTS audio graphics window system REQUIRED)

target_link_libraries(protolink
        PUBLIC
        pthread
        sfml-graphics sfml-audio sfml-window sfml-system )