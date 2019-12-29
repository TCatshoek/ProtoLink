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

#define buflen 1024

namespace {
    uint window_w = 1000;
    uint window_h = 400;

    uint8_t buf[buflen];

    bool shouldStop = false;

    TCPServer* server_p;
    TCPSocket* socket_p;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void handleNetwork(uint16_t port) {
    // Setup protolink
    ProtoLink<TCPSocket> protolink(buf, buflen);

    // Setup neworking
    TCPServer server(8080);
    server_p = &server;

    while (!shouldStop) {
        // Wait for connection
        TCPSocket socket = server.accept();
        socket_p = &socket;
        std::cout << "Client connected from " << socket.getAddress() << ":" << socket.getPort() << std::endl;

        // Todo: figure out timeout stuff
        //socket.setTimeout(10, 0);
        //socket.setBlocking(false);

        while (socket.connected() && !shouldStop) {
            // Keep track of protolink state
            ProtoLink<TCPSocket>::State state;
            state = protolink.init(&socket);

            std::cout << "Protolink: " << protolink.stateToString() << std::endl;

            // Step protolink while we can
            while (socket.connected()
                    && state != ProtoLink<TCPSocket>::State::ERR
                    && !shouldStop) {

                state = protolink.run();
                std::cout << "Protolink: " << protolink.stateToString() << std::endl;

                if (socket.connected())
                    socket.write((char)state);
            }
        }

        socket.close();

        std::cout << "Client disconnected" << std::endl;
    }

    server.close();
}
#pragma clang diagnostic pop

bool getbit(const uint8_t* buf, int pos) {
    int byteidx = pos / (sizeof(uint8_t) * 8);
    int bitidx = pos % (sizeof(uint8_t) * 8);

    uint8_t tmp = buf[byteidx];

    int bit = (tmp >> bitidx) & 1;

    return (bool)bit;
}

void drawFb(Matrix* matrices[], uint n_matrices) {
    int acc = 0;

    for (uint i = 0; i < n_matrices; i++) {
        Matrix* m = matrices[i];

        for (int y = 0; y < m->h; y++) {
            for(int x = 0; x < m->w; x++) {
                m->drawPixel(x, y, getbit(buf, acc + x + y * m->w));
            }
        }

        acc += m->w * m->h;
    }
}

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
                          &mouth_l_b, &mouth_l_f, &mouth_r_f, &mouth_r_b};


    // create the window
    sf::RenderWindow window(sf::VideoMode(window_w, window_h), "My window", sf::Style::Titlebar | sf::Style::Close);

    window.setFramerateLimit(60);
    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed ||
                    (event.type == sf::Event::KeyPressed
                    && event.key.code == sf::Keyboard::Escape)){
                std::cout << "closing" << std::endl;
                window.close();
                // Doesn't really work yet
                shouldStop = true;
            }
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw everything here...
        drawFb(matrices, 8);

        for(auto matrix: matrices) {
            matrix->render(window);
        }


        // end the current frame
        window.display();
    }

    return 0;
}