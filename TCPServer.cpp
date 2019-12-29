//
// Created by tom on 12/29/19.
//

#include "TCPServer.h"

TCPServer::TCPServer(uint16_t port) {
    // Open a socket and get it's file descriptor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        perror("ERROR opening socket");
        exit(-1);
    }

    // Set up address struct containing the
    // address and port to listen on
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    // Bind the socket to it's address
    int bind_r = bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (bind_r < 0) {
        perror("ERROR on binding");
        exit(-1);
    }

    listen(sockfd, 0);
}

TCPSocket TCPServer::accept() {
    return TCPSocket(sockfd);
}

int TCPServer::close() {
    return ::close(sockfd);
}

TCPServer::~TCPServer() {
    TCPServer::close();
}
