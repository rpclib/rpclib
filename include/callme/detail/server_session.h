#pragma once

#ifndef SESSION_H_5KG6ZMAB
#define SESSION_H_5KG6ZMAB

#include "asio.hpp"
#include "callme/detail/log.h"
#include "msgpack.hpp"
#include <memory>
#include <vector>

#include "callme/dispatcher.h"

namespace callme {
namespace detail {

class server_session : public std::enable_shared_from_this<server_session> {
public:
    server_session(asio::io_service *io, asio::ip::tcp::socket socket,
                   std::shared_ptr<dispatcher> disp);
    void start();

private:
    void do_read();
    void do_write();
    void write(msgpack::sbuffer&& resp);

private:
    asio::io_service* io_;
    asio::ip::tcp::socket socket_;
    asio::strand write_strand_, read_strand_;
    std::shared_ptr<dispatcher> disp_;
    std::deque<msgpack::sbuffer> write_queue_;
    msgpack::unpacker pac_;
    msgpack::sbuffer output_buf_;
    static const uint32_t default_buffer_size = 4096;
    CALLME_CREATE_LOG_CHANNEL(session)
};
} /* detail */
} /* callme */

#endif /* end of include guard: SESSION_H_5KG6ZMAB */
