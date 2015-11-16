#include "callme/dispatcher.h"

namespace callme {

void dispatcher::dispatch(msgpack::sbuffer const &msg) {
    msgpack::unpacked unpacked;
    msgpack::unpack(&unpacked, msg.data(), msg.size());
    auto o = unpacked.get();

    msg_type the_call;
    o.convert(&the_call);

    auto it_func = funcs_.find(std::get<2>(the_call));
    if (it_func != end(funcs_)) {
        (it_func->second)(std::get<3>(the_call));
    }
}

} /* callme */
