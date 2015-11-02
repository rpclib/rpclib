#include "callme.h"

#include <iostream>

using namespace boost::asio;

namespace callme
{

server::server()
{
    std::cout << "Server created" << std::endl;
}

} /* callme */
