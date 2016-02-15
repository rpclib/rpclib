#include "callme/server.h"
#include <stdexcept>

#include "callme/detail/dev_utils.h"
#include "callme/detail/log.h"
#include "callme/detail/server_session.h"
#include "format.h"

using namespace callme::detail;
using asio::ip::tcp;

namespace callme {

server::server(std::string const &address, uint16_t port)
    : io_(),
      acceptor_(io_, tcp::endpoint(tcp::v4(), port)),
      socket_(io_),
      disp_(std::make_shared<dispatcher>()),
      suppress_exceptions_(false) {
    LOG_INFO("Created server on address {}:{}", address, port);
    start_accept();
}

server::~server() {
    io_.stop();
    loop_workers_.join_all();
}

void server::suppress_exceptions(bool suppress) {
    suppress_exceptions_ = suppress;
}

void server::run() { io_.run(); }

void server::async_run(std::size_t worker_threads) {
    loop_workers_.create_threads(worker_threads, [this]() {
        name_thread("server");
        LOG_INFO("Starting");
        io_.run();
        LOG_INFO("Exiting");
    });
}

void server::start_accept() {
    acceptor_.async_accept(socket_, [this](std::error_code ec) {
        if (!ec) {
            LOG_INFO("Accepted connection.");
            std::make_shared<server_session>(&io_, std::move(socket_), disp_,
                                             suppress_exceptions_)
                ->start();
        } else {
            // TODO: handle error [sztomi 2016-01-13]
        }
        start_accept();
        // TODO: allow graceful exit [sztomi 2016-01-13]
    });
}

} /* callme */
