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

    impl(server *parent, uint16_t port)
        : parent_(parent),
          io_(),
          acceptor_(io_, tcp::endpoint(tcp::v4(), port)),
          socket_(io_),
          suppress_exceptions_(false) {}

    void start_accept() {
        acceptor_.async_accept(socket_, [this](std::error_code ec) {
            if (!ec) {
                LOG_INFO("Accepted connection.");
                auto s = std::make_shared<server_session>(
                    parent_, &io_, std::move(socket_), parent_->disp_,
                    suppress_exceptions_);
                s->start();
                sessions_.push_back(s);
            } else {
                LOG_ERROR("Error while accepting connection: {}", ec);
            }
            start_accept();
            // TODO: allow graceful exit [sztomi 2016-01-13]
        });
    }

    void close_sessions() {
        for (auto &session : sessions_) {
            session->close();
        }
        sessions_.clear();
    }

    void stop() {
        io_.stop();
        loop_workers_.join_all();
    }

    server *parent_;
    io_service io_;
    ip::tcp::acceptor acceptor_;
    ip::tcp::socket socket_;
    callme::detail::thread_group loop_workers_;
    std::vector<std::shared_ptr<server_session>> sessions_;
    std::atomic_bool suppress_exceptions_;
    CALLME_CREATE_LOG_CHANNEL(server)
};

CALLME_CREATE_LOG_CHANNEL(server)

server::server(uint16_t port)
    : pimpl(this, port), disp_(std::make_shared<dispatcher>()) {
    LOG_INFO("Created server on localhost:{}", port);
    pimpl->start_accept();
}

server::server(std::string const &address, uint16_t port)
    : pimpl(this, address, port), disp_(std::make_shared<dispatcher>()) {
    LOG_INFO("Created server on address {}:{}", address, port);
    pimpl->start_accept();
}

server::~server() { pimpl->stop(); }

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

void server::stop() { pimpl->stop(); }

void server::close_sessions() { pimpl->close_sessions(); }

} /* callme */
