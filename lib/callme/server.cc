#include "callme/server.h"

#include <boost/asio.hpp>

namespace callme
{

server::server()
    : io_(std::make_unique<boost::asio::io_service>())
{
}

} /* callme */
