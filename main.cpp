//
// Created by tom on 12/28/19.
//
#include <SFML/Graphics.hpp>
#include "matrix.h"
#include <thread>
#include "protolink.h"
#include "tcpwrapper.h"

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

#define buflen 1024

#define PORT 8080

namespace {
    uint window_w = 1000;
    uint window_h = 400;

    uint8_t buf[buflen];

    bool shouldStop = false;
}

void handleNetwork(uint16_t port) {
    // Setup protolink
    ProtoLink<TCPWrapper> protolink(buf, buflen);
    try {
        // Create boost IO service
        boost::asio::io_service io_service;

        // Start listening
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), PORT));

        while (!shouldStop) {
            try{
                // Wait for client to connect
                tcp::socket socket(io_service);
                acceptor.accept(socket);

                // Wrap the boost socket so protolink knows how to talk to it
                TCPWrapper sock(socket);

                // Keep track of protolink state
                ProtoLink<TCPWrapper>::State state;
                state = protolink.init(&sock);
                std::cout << "Protolink: " << protolink.stateToString() << std::endl;

                while (state != ProtoLink<TCPWrapper>::State::ERR) {
                    protolink.run();
                    std::cout << "Protolink: " << protolink.stateToString() << std::endl;

                    sock.write((char)state);
                }
            }
            catch (boost::system::system_error& e) {
                if (e.code() == boost::asio::error::connection_reset) {
                    std::cerr << "Client disconnected: " << e.what() << std::endl;
                } else {
                    throw e;
                }

            }
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }

}

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
    sf::RenderWindow window(sf::VideoMode(window_w, window_h), "Beeper simulator v0.0.1", sf::Style::Titlebar | sf::Style::Close);

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
                net.join();
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