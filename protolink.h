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

    int min(int a, int b) {
        return a < b ? a : b;
    }

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
    enum class State {
        NO_CONN,
        INIT,
        RECV_FB,
        ERR
    };

    enum class MsgType {
        RECV_FB,
        AREYOUTHERE,
        YESIMHERE,
    };

private:
    Connection* con;
    uint8_t* fb;
    uint fb_size;

    // Initial state
    State state = State::NO_CONN;

public:
    ProtoLink(uint8_t* fbp, uint fbs){
        fb = fbp;
        fb_size = fbs;
    }

    State init(Connection* c) {
        con = c;
        state = State::INIT;
        return state;
    }

    State run() {
        switch(state) {
            case State::NO_CONN: break;
            case State::INIT:    run_INIT();     break;
            case State::RECV_FB: run_RECV_FB();  break;
        }
        return state;
    }

    String stateToString() {
        switch (state) {
            to_str(State::NO_CONN);
            to_str(State::INIT);
            to_str(State::RECV_FB);
            to_str(State::ERR);
        }
        return "Unknown"; // Unreachable, yet clang complains
    }

private:
    // State methods
    void run_INIT() {
        auto in = (MsgType)con->read();

        switch(in) {
            case MsgType::RECV_FB:
                state = State::RECV_FB;
                break;

            case MsgType::AREYOUTHERE:
                con->write((char)MsgType::YESIMHERE);
                break;

            default:
                state = State::ERR;
        };
    }

    void run_RECV_FB() {

        // First LEN_N_BYTES bytes are body length in bytes
        uint len = 0;

        uint8_t lenbuf[LEN_N_BYTES];
        uint b_read = con->readBytes(lenbuf, LEN_N_BYTES);

        if (b_read == 0) {
            return;
        }

        // Assemble bytes into int
        for (int i = 0; i < LEN_N_BYTES; i++) {
            len |= (lenbuf[LEN_N_BYTES - 1 - i] << i * 8);
        }

        // Copy the rest of the message into the framebuffer
        con->readBytes(fb, min(fb_size, len));

        // Go back to initial state to wait for next command
        state = State::INIT;
    }
};

#endif //PROTOLINK_PROTOLINK_H
