#include "callme/dispatcher.h"
#include "format.h"

namespace callme {

void dispatcher::dispatch(msgpack::sbuffer const &msg) {
    msgpack::unpacked unpacked;
    msgpack::unpack(&unpacked, msg.data(), msg.size());
    dispatch(unpacked.get());
}

response dispatcher::dispatch(msgpack::object const &msg) {
    msg_type the_call;
    msg.convert(&the_call);

    auto &&type = std::get<0>(the_call);
    auto &&id = std::get<1>(the_call);
    auto &&name = std::get<2>(the_call);
    auto &&args = std::get<3>(the_call);

    auto it_func = funcs_.find(name);
    if (it_func != end(funcs_)) {
        auto result = (it_func->second)(std::get<3>(the_call));
        return response(id, boost::string_ref(), std::move(result));
    } else {
        return response(id, fmt::format("callme: server could not find "
                                        "function '{0}' with argument count {1}.",
                                        name, args.via.array.size),
                        std::make_unique<msgpack::object>());
    }
}

} /* callme */
