cmake_minimum_required(VERSION 3.12)
project(protolink)

set(CMAKE_C_STANDARD 11)

add_executable(protolink
        main.cpp
        protolink.h
        matrix.h matrix.cpp
        tcpwrapper.cpp tcpwrapper.h
        )

find_package(SFML 2.5 COMPONENTS audio graphics window system REQUIRED)

find_package(Boost COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

target_link_libraries(protolink
        PUBLIC
        pthread
        ${Boost_LIBRARIES}
        sfml-graphics sfml-audio sfml-window sfml-system )