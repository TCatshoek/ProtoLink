//
// Created by tom on 12/29/19.
//

#ifndef PROTOLINK_TCPSERVER_H
#define PROTOLINK_TCPSERVER_H

#include <cstdint>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <libnet.h>

#include "TCPSocket.h"

// Dead simple tcp socket server wrapper
// Just blows up in your face if anything goes wrong

class TCPServer {
private:
    int sockfd;
    struct sockaddr_in serv_addr = {};

public:
    TCPServer(uint16_t port);

    TCPSocket accept();

    int close();

    ~TCPServer();

};


#endif //PROTOLINK_TCPSERVER_H
