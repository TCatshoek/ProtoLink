cmake_minimum_required(VERSION 3.12)
project(protolink)

set(CMAKE_C_STANDARD 11)

# Point this to correct nanopb folder
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/protobuf/nanopb/extra)

find_package(Nanopb REQUIRED)
include_directories(${NANOPB_INCLUDE_DIRS})

nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS protobuf/ProtoLink_protobuf/protolink.proto)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS}
        PROPERTIES GENERATED TRUE)

add_executable(protolink
        main.cpp
        protolink.h
        matrix.h matrix.cpp
        tcpwrapper.cpp tcpwrapper.h
        ${PROTO_SRCS} ${PROTO_HDRS} )

find_package(SFML 2.5 COMPONENTS audio graphics window system REQUIRED)

find_package(Boost COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

target_link_libraries(protolink
        PUBLIC
        pthread
        ${Boost_LIBRARIES}
        sfml-graphics sfml-audio sfml-window sfml-system )