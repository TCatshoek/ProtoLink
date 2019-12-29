//
// Created by tom on 12/28/19.
//
#include <SFML/Graphics.hpp>
#include "matrix.h"
#include <thread>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <libnet.h>

#include "TCPSocket.h"
#include "TCPServer.h"
#include "protolink.h"

namespace {
    uint window_w = 1000;
    uint window_h = 400;

    uint buflen = 1024;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void handleNetwork(uint16_t port) {
    // Setup protolink
    uint8_t buf[buflen];
    ProtoLink<TCPSocket> protolink(buf, buflen);

    // Setup neworking
    TCPServer server(8080);

    while (true) {
        TCPSocket socket = server.accept();
        std::cout << "Client connected from " << socket.getAddress() << ":" << socket.getPort() << std::endl;

        while (socket.connected()) {
            // Keep track of protolink state
            ProtoLink<TCPSocket>::State state;
            state = protolink.init(&socket);

            std::cout << "Protolink: " << protolink.stateToString() << std::endl;

            // Step protolink while we can
            while (socket.connected() && state != ProtoLink<TCPSocket>::State::ERR) {
                state = protolink.run();
                std::cout << "Protolink: " << protolink.stateToString() << std::endl;
                socket.write(state);
            }

        }


        std::cout << "Client disconnected" << std::endl;
    }
}
#pragma clang diagnostic pop

int main (int argc, char *argv[]) {
    // Start networking thread
    std::thread net(handleNetwork, 8080);

    // Setup "LED Matrices"
    Matrix eye_l(16, 8, 10.f, 10.f, false);
    Matrix eye_r(16, 8, window_w - 16 * 15.f - 5, 10.f, false);

    Matrix nose_l(8, 8, 0.4f * window_w - 4 * 15.f, window_h * 0.3f , false);
    Matrix nose_r(8, 8, 0.6f * window_w - 4 * 15.f, window_h * 0.3f, false);

    Matrix mouth_l_b(16, 8, 10.f, window_h - 8 * 15.f - 5, false);
    Matrix mouth_l_f(16, 8, 10.f + 16 * 15.f, window_h - 8 * 15.f - 5, false);

    Matrix mouth_r_b(16, 8, window_w - 5.f - 16 * 15.f, window_h - 8 * 15.f - 5, false);
    Matrix mouth_r_f(16, 8, window_w - 5.f - 32 * 15.f, window_h - 8 * 15.f - 5, false);

    Matrix* matrices[] = {&eye_l, &eye_r, &nose_l, &nose_r,
                          &mouth_l_b, &mouth_l_f, &mouth_r_b, &mouth_r_f};

    eye_l.drawPixel(15, 7, true);

    // create the window
    sf::RenderWindow window(sf::VideoMode(window_w, window_h), "My window", sf::Style::Titlebar | sf::Style::Close);

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw everything here...
        for(auto matrix: matrices) {
            matrix->render(window);
        }


        // end the current frame
        window.display();
    }

    return 0;
}