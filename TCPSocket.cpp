//
// Created by tom on 12/29/19.
//

#include "TCPSocket.h"

TCPSocket::TCPSocket(int sockfd) {
    clisockfd = accept(sockfd, (struct sockaddr*)&client_addr, &clilen);

    if (clisockfd < 0) {
        perror("ERROR on accept");
        exit(-1);
    }

    _connected = true;
}

bool TCPSocket::connected() {
    if (!_connected) {
        return false;
    }

    int error = 0;
    socklen_t len = sizeof (error);

    getsockopt (clisockfd, SOL_SOCKET, SO_ERROR, &error, &len);

    return error == 0;
}

int TCPSocket::available() {
    int bytesAvailable;
    ioctl(clisockfd, FIONREAD, &bytesAvailable);
    return bytesAvailable;
}

int TCPSocket::getPort() {
    return ntohs(client_addr.sin_port);
}

std::string TCPSocket::getAddress() {
    return std::string(inet_ntoa(client_addr.sin_addr));
}

ssize_t TCPSocket::readBytes(void* buf, uint len) {
    ssize_t n = ::read(clisockfd, buf, len);

    if (n < 0) {
        perror("ERROR reading from socket");
        exit(-1);
    }

    if (n == 0) {
        _connected = false;
    }

    return n;
}

int TCPSocket::read() {
    char buf[1];
    ssize_t n = readBytes(buf, 1);

    if (n < 0) {
        return -1;
    } else {
        return (int)buf[0];
    }
}

int TCPSocket::write(char c) {
    ssize_t n = ::write(clisockfd, &c, 1);

    if (n < 0) {
        perror("Error writing to socket");
    } else {
        return (int)n;
    }
}
