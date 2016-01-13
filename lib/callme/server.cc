#include "callme/server.h"
#include <stdexcept>

#include "callme/detail/session.h"
#include "callme/detail/log.h"
#include "callme/detail/uv_loop.h"
#include "format.h"

static inline bool is_success(int result) { return result == 0; }
static inline bool is_error(int result) { return result < 0; }

using namespace callme::detail;
using asio::ip::tcp;

namespace callme {

server::server(std::string const &address, uint16_t port)
    : io_(),
      acceptor_(io_, tcp::endpoint(tcp::v4(), port)),
      socket_(io_),
      disp_(std::make_shared<dispatcher>()),
      suppress_exceptions_(false) {
    LOG_INFO("Created server on address %v:%v", address, port);
}

void server::suppress_exceptions(bool suppress) {
    suppress_exceptions_ = suppress;
}

void server::start() {
    io_.run();
}

void server::run() {
    acceptor_.async_accept(socket_, [this](std::error_code ec) {
        if (!ec) {
            LOG_INFO("Accepted connection.");
            std::make_shared<session>(std::move(socket_), disp_)->start();
        } else {
            // TODO: handle error [sztomi 2016-01-13]
        }
        run();
        // TODO: allow graceful exit [sztomi 2016-01-13]
    });
}

} /* callme */
