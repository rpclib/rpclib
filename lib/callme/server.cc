#include "callme/server.h"

#include <atomic>
#include <memory>
#include <stdexcept>
#include <stdint.h>
#include <thread>

#include "asio.hpp"
#include "format.h"

#include "callme/detail/dev_utils.h"
#include "callme/detail/log.h"
#include "callme/detail/log.h"
#include "callme/detail/server_session.h"
#include "callme/detail/thread_group.h"

using namespace callme::detail;
using CALLME_ASIO::ip::tcp;
using namespace CALLME_ASIO;

namespace callme {

struct server::impl {
    impl(server *parent, std::string const &address, uint16_t port)
        : parent_(parent),
          io_(),
          acceptor_(io_,
                    tcp::endpoint(ip::address::from_string(address), port)),
          socket_(io_),
          suppress_exceptions_(false) {}

    void start_accept() {
        acceptor_.async_accept(socket_, [this](std::error_code ec) {
            if (!ec) {
                LOG_INFO("Accepted connection.");
                std::make_shared<server_session>(&io_, std::move(socket_),
                                                 parent_->disp_,
                                                 suppress_exceptions_)
                    ->start();
            } else {
                LOG_ERROR("Error while accepting connection: {}", ec);
            }
            start_accept();
            // TODO: allow graceful exit [sztomi 2016-01-13]
        });
    }

    server *parent_;
    CALLME_ASIO::io_service io_;
    CALLME_ASIO::ip::tcp::acceptor acceptor_;
    CALLME_ASIO::ip::tcp::socket socket_;
    callme::detail::thread_group loop_workers_;
    std::atomic_bool suppress_exceptions_;
    CALLME_CREATE_LOG_CHANNEL(server)
};

server::server(std::string const &address, uint16_t port)
    : pimpl(this, address, port), disp_(std::make_shared<dispatcher>()) {
    LOG_INFO("Created server on address {}:{}", address, port);
    pimpl->start_accept();
}

server::~server() {
    pimpl->io_.stop();
    pimpl->loop_workers_.join_all();
}

void server::suppress_exceptions(bool suppress) {
    pimpl->suppress_exceptions_ = suppress;
}

void server::run() { pimpl->io_.run(); }

void server::async_run(std::size_t worker_threads) {
    pimpl->loop_workers_.create_threads(worker_threads, [this]() {
        name_thread("server");
        LOG_INFO("Starting");
        pimpl->io_.run();
        LOG_INFO("Exiting");
    });
}

} /* callme */
