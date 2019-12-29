//
// Created by tom on 12/29/19.
//

#ifndef PROTOLINK_TCPSOCKET_H
#define PROTOLINK_TCPSOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <libnet.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <string>

// Dead simple tcp socket wrapper
// Just blows up in your face if anything goes wrong

class TCPSocket {
private:
    struct sockaddr_in client_addr = {};
    uint clilen = sizeof(client_addr);
    int clisockfd;
    bool _connected = false;

public:
    explicit TCPSocket(int sockfd);

    bool connected();

    int available();

    int getPort();
    std::string getAddress();

    ssize_t readBytes(void* buf, uint len);

    int read();

    int write(char);

    void setTimeout(int sec, int usec);

    void setBlocking(bool enabled);

    ~TCPSocket();

    int close();
};


#endif //PROTOLINK_TCPSOCKET_H
