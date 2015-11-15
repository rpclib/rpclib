#include "callme/server.h"

#include <iostream>

// including parts of asio is annoying, but saves compilation time
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <boost/asio/ip/tcp.hpp>

using boost::asio::io_service;

namespace callme {

server::server(boost::string_ref address, int16_t port)
    : io_(std::make_shared<io_service>()) {}

void server::process_call(msgpack::sbuffer const &msg) {
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
