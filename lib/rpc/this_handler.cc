#include "rpc/this_handler.h"

namespace rpc {

this_handler_t &this_handler() {
    static thread_local this_handler_t instance;
    return instance;
}

void this_handler_t::disable_response() { resp_enabled_ = false; }

void this_handler_t::enable_response() { resp_enabled_ = true; }

void this_handler_t::clear() {
    error_.set(msgpack::object());
    resp_.set(msgpack::object());
    enable_response();
}

} /* rpc */
