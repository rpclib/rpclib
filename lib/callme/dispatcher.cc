#include "callme/dispatcher.h"

#include <iostream>

namespace callme {

void dispatcher::process_call(msgpack::sbuffer const &msg) {
    msgpack::unpacked unpacked;
    msgpack::unpack(&unpacked, msg.data(), msg.size());
    auto o = unpacked.get();

    msg_type the_call;
    o.convert(&the_call);

    std::cout << o << std::endl;

    auto it_func = funcs_.find(std::get<2>(the_call));
    if (it_func != end(funcs_)) {
        (it_func->second)(std::get<3>(the_call));
    }
}

} /* callme */
