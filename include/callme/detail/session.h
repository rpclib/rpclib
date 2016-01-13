#pragma once

#ifndef SESSION_H_5KG6ZMAB
#define SESSION_H_5KG6ZMAB

#include <memory>
#include "asio.hpp"
#include "msgpack.hpp"

#include "callme/dispatcher.h"

namespace callme {
namespace detail {

class session : public std::enable_shared_from_this<session> {
public:
    session(asio::ip::tcp::socket socket, std::shared_ptr<dispatcher> disp);
    void start();

private:
    void do_read();

private:
    asio::ip::tcp::socket socket_;
    std::shared_ptr<dispatcher> disp_;
    msgpack::unpacker pac_;
    msgpack::sbuffer buf_;
    static const uint32_t default_buffer_size = 4096;
};
} /* detail */
} /* callme */

#endif /* end of include guard: SESSION_H_5KG6ZMAB */
