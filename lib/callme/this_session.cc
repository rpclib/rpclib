#include "callme/this_session.h"

namespace callme
{

this_session_t &this_session() {
    static thread_local this_session_t instance;
    return instance;
}

void this_session_t::post_exit() {
    exit_ = true;
}

void this_session_t::clear() {
    exit_ = false;
}

    
} /* callme */ 
