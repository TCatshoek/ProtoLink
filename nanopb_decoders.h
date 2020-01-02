//
// Created by tom on 1/2/20.
//

#ifndef PROTOLINK_NANOPB_DECODERS_H
#define PROTOLINK_NANOPB_DECODERS_H

#include <pb_common.h>
#include <pb_decode.h>
#include "protolink.pb.h"
#include "protolink.h"

template <class Connection>
class ProtoLink;

template <class Connection>
enum class ProtoLink<Connection>::State;

// Function to directly write the incoming framebuffer message to the framebuffer
template <class Connection>
bool write_fb(pb_istream_t *stream, const pb_field_t *field, void **arg) {
    auto* p = (ProtoLink<Connection>*) arg[0];

    if (stream->bytes_left > p->fb_size) {
        p->state = p->ProtoLink<Connection>::State::ERR;
        p->error_msg = "New framebuffer data exceeds framebuffer size";
        return false;
    }

    return pb_read(stream, p->fb, stream->bytes_left);
}

// Function to determine message type
template <class Connection>
bool command_decode_callback(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    auto* p = (ProtoLink<Connection>*) arg[0];

    auto msg = (Command*)field->message;

    switch (field->tag) {
        case Command_updatefb_tag:
            auto submsg = (UpdateFB*)field->pData;
            submsg->fb.funcs.decode = write_fb<Connection>;
            submsg->fb.arg = arg;
    };

    return true;
}


#endif //PROTOLINK_NANOPB_DECODERS_H
