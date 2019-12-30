//
// Created by tom on 12/28/19.
//

#include "tcpwrapper.h"

TCPWrapper::TCPWrapper(tcp::socket &socket): socket(socket) {

}

int TCPWrapper::read() {
    char buf[1];

    size_t n = readBytes(&buf, 1);

    if (n < 0) {
        return -1;
    } else {
        return (int)buf[0];
    }
}

size_t TCPWrapper::readBytes(void *buf, uint len) {
    boost::asio::mutable_buffer bbuf = boost::asio::buffer(buf, len);
    size_t n = boost::asio::read(socket, bbuf,
                      boost::asio::transfer_exactly(len));
    return n;
}

int TCPWrapper::write(char c) {
    std::string msg(&c);

    boost::system::error_code ignored_error;
    size_t n = boost::asio::write(socket, boost::asio::buffer(msg),
                       boost::asio::transfer_all(), ignored_error);

    return (int)n;
}
