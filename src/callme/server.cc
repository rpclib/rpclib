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
    : io_(std::make_shared<io_service>())
{
}

} /* callme */
