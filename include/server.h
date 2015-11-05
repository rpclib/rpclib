#pragma once

#ifndef CALLME_H_NBGBTUFL
#define CALLME_H_NBGBTUFL

#include <memory>

namespace boost {
namespace asio {
    class io_service;
}
}

namespace callme {

//!
class server {
public:
    server();

private:
    std::shared_ptr<boost::asio::io_service> service_;
};
}

#endif /* end of include guard: CALLME_H_NBGBTUFL */
