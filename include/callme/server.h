#pragma once

#ifndef SERVER_H_S0HB5KXY
#define SERVER_H_S0HB5KXY

#include <memory>

namespace boost {
namespace asio {
class io_service;
}
}

namespace callme {

class server {
public:
    server();

    void run();

private:
    std::unique_ptr<boost::asio::io_service> io_;
};

} /* callme */

#endif /* end of include guard: SERVER_H_S0HB5KXY */
