cmake_minimum_required(VERSION 3.12)
project(protolink)

set(CMAKE_C_STANDARD 11)

set(NANOPB
        protobuf/nanopb/pb.h
        protobuf/nanopb/pb_common.h
        protobuf/nanopb/pb_common.c
        protobuf/nanopb/pb_encode.h
        protobuf/nanopb/pb_encode.c
        protobuf/nanopb/pb_decode.h
        protobuf/nanopb/pb_decode.c
        protobuf/nanopb/protolink.pb.h
        protobuf/nanopb/protolink.pb.c
        )
include_directories(protobuf/nanopb)

add_executable(protolink
        main.cpp
        protolink.h
        matrix.h matrix.cpp
        tcpwrapper.cpp tcpwrapper.h
        ${NANOPB}
        )

find_package(SFML 2.5 COMPONENTS audio graphics window system REQUIRED)

find_package(Boost COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

target_link_libraries(protolink
        PUBLIC
        pthread
        ${Boost_LIBRARIES}
        sfml-graphics sfml-audio sfml-window sfml-system )