//
// Created by tom on 12/28/19.
//

#ifndef PROTOLINK_PROTOLINK_H
#define PROTOLINK_PROTOLINK_H

#ifndef ESP8266
    typedef unsigned int uint;
    #include <cstdint>
    #include <string>
    #include <algorithm>
    #define YIELD
    #define String std::string
    #define to_str(p) case(p): return std::string(#p);
    #define min std::min
#endif
#ifdef ESP8266
    #define YIELD yield();
    #define String String
    #define to_str(p) case(p): return #p;
    #define min min
#endif

#define LEN_N_BYTES 2

template <class Connection>
class ProtoLink {
public:
    enum State {
        NO_CONN,
        INIT,
        RECV_FB,
        ERR
    };

private:
    Connection* con;
    uint8_t* fb;
    uint fb_size;

    // Initial state
    State state = NO_CONN;

public:
    ProtoLink(uint8_t* fbp, uint fbs){
        fb = fbp;
        fb_size = fbs;
    }

    State init(Connection* c) {
        con = c;
        state = INIT;
        return state;
    }

    State run() {
        switch(state) {
            case NO_CONN: break;
            case INIT:    run_INIT();     break;
            case RECV_FB: run_RECV_FB();  break;
        }
        return state;
    }

    String stateToString() {
        switch (state) {
            to_str(NO_CONN);
            to_str(INIT);
            to_str(RECV_FB);
            to_str(ERR);
        }
        return "Unknown";
    }

private:
    // State methods
    void run_INIT() {
        while (con->connected()) {
            if (con->available()) {
                int in = con->read();

                switch(in) {
                    case RECV_FB: state = RECV_FB; break;
                    default: state = ERR; break;
                };

                return;
            }
            YIELD
        }
    }

    void run_RECV_FB() {

        // First LEN_N_BYTES bytes are body length in bytes
        uint len = 0;
        while (!len && con->connected()) {
            if (con->available() >= LEN_N_BYTES) {

                uint8_t lenbuf[LEN_N_BYTES];
                con->readBytes(lenbuf, LEN_N_BYTES);

                for (int i = 0; i < LEN_N_BYTES; i++) {
                    len |= (lenbuf[LEN_N_BYTES - 1 - i] << i * 8);
                }
            }
            YIELD
        }

        // Copy the rest of the message into the framebuffer
        while (con->connected()) {

            // TODO:: Should check if remote is still open,
            // Might get stuck otherwise

            int available = con->available();

            if (con->available() >= len) {

                // No buffer overflow pls
                int copy_len = min(fb_size, len);

                con->readBytes(fb, copy_len);

                // Go back to initial state to wait for next command
                state = INIT;
                return;
            }
            YIELD
        }
    }
};

#endif //PROTOLINK_PROTOLINK_H
