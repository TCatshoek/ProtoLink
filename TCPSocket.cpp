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
        TCPSocket::close();
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
    ssize_t n = ::send(clisockfd, &c, 1, MSG_NOSIGNAL);

    if (n < 0) {
        perror("Error writing to socket");
        return TCPSocket::close();
    } else {
        return (int)n;
    }
}

int TCPSocket::close() {
    _connected = false;
    return ::close(clisockfd);
}

TCPSocket::~TCPSocket() {
    TCPSocket::close();
}

void TCPSocket::setTimeout(int sec, int usec) {
    struct timeval timeout = {};
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;

    if (setsockopt (clisockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                    sizeof(timeout)) < 0) {
        perror("Setting timeout failed");
    }
}

void TCPSocket::setBlocking(bool enabled) {
    int x;
    x=fcntl(clisockfd ,F_GETFL, 0);

    if (enabled)
        fcntl(clisockfd, F_SETFL, x & ~O_NONBLOCK);
    else
        fcntl(clisockfd, F_SETFL, x | O_NONBLOCK);
}


