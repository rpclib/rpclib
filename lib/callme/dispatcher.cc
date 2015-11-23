#include "callme/dispatcher.h"

namespace callme {

void dispatcher::dispatch(msgpack::sbuffer const &msg) {
    msgpack::unpacked unpacked;
    msgpack::unpack(&unpacked, msg.data(), msg.size());
    dispatch(unpacked.get());
}

void dispatcher::dispatch(msgpack::object const &msg) {
    msg_type the_call;
    msg.convert(&the_call);

    auto id = std::get<1>(the_call);

    auto it_func = funcs_.find(std::get<2>(the_call));
    if (it_func != end(funcs_)) {
        auto result = (it_func->second)(std::get<3>(the_call));
    }
}

} /* callme */
