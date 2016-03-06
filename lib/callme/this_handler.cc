#include "callme/this_handler.h"

namespace callme
{

this_handler_t& this_handler()
{
    static thread_local this_handler_t instance;
    return instance;
}

void this_handler_t::clear_error() {
    error_.set(msgpack::object());
}

void this_handler_t::clear_special_response() {
    resp_.set(msgpack::object());
}

void this_handler_t::disable_response() {
    resp_enabled_ = false;
}

void this_handler_t::enable_response() {
    resp_enabled_ = true;
}

void this_handler_t::clear() {
    clear_error();
    clear_special_response();
    enable_response();
}
    
} /* callme */ 
