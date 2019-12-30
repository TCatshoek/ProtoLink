//
// Created by tom on 12/28/19.
//

#ifndef PROTOLINK_TCPWRAPPER_H
#define PROTOLINK_TCPWRAPPER_H

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

class TCPWrapper {
private:
    tcp::socket &socket;

public:
    TCPWrapper(tcp::socket &socket);

    int read();

    size_t readBytes(void* buf, uint len);

    int write(char);

};


#endif //PROTOLINK_TCPWRAPPER_H
