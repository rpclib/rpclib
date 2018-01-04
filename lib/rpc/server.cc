#include "rpc/server.h"

#include <stdint.h>
#include <atomic>
#include <memory>
#include <stdexcept>
#include <thread>

#include <uv.h>

#include "rpc/detail/uv_adaptor.h"
#include "rpc/detail/uv_loop.h"

#include "asio.hpp"
#include "format.h"

#include "rpc/detail/dev_utils.h"
#include "rpc/detail/log.h"
#include "rpc/detail/make_unique.h"
#include "rpc/detail/server_session.h"
#include "rpc/detail/thread_group.h"
#include "rpc/detail/uv_tools.h"

using namespace rpc::detail;
using RPCLIB_ASIO::ip::tcp;
using namespace RPCLIB_ASIO;

namespace rpc {

struct server::impl : uv_adaptor<server::impl> {
  impl(server *parent, std::string const &address, uint16_t port)
      : parent_(parent), suppress_exceptions_(false) {
    auto loop = loop_.get();
    uv_tcp_init(loop, &socket_);
    sockaddr_in bind_addr = {};
    if (uv_ip4_addr(address.c_str(), port, &bind_addr) < 0) {
      // TODO(sztomi) handle error
      throw std::runtime_error("uv_ip4_addr fail");
    }
    if (uv_tcp_bind(&socket_, reinterpret_cast<sockaddr *>(&bind_addr), 0) <
        0) {
      // TODO(sztomi) handle error
      throw std::runtime_error("uv_tcp_bind fail");
    }
    socket_.data = this;
    int r = uv_listen(reinterpret_cast<uv_stream_t *>(&socket_),
                      /*backlog=*/128, &fw_on_new_connection);
    if (r < 0) {
      // TODO(sztomi) handle error
      throw std::runtime_error(
          RPCLIB_FMT::format("uv_listen fail: {}", uv_strerror(r)).c_str());
    }

    LOG_INFO("Listening.");
  }

  void on_new_connection(uv_stream_t *srv_handle, int status) {
    LOG_INFO("Accepting new connection.");
    check_uv("on_new_connection", status);

    auto client_socket = std::unique_ptr<uv_tcp_t>(new uv_tcp_t);
    check_uv("uv_tcp_init for client socket",
             uv_tcp_init(loop_.get(), client_socket.get()));

    check_uv("uv_accept for client connection",
             uv_accept(reinterpret_cast<uv_stream_t *>(&socket_),
                       reinterpret_cast<uv_stream_t *>(client_socket.get())));
    sessions_.push_back(detail::make_unique<server_session>(
        parent_, loop_.get(), std::move(client_socket), parent_->disp_,
        suppress_exceptions_));
  }

  void run() { uv_run(loop_.get(), UV_RUN_DEFAULT); }

  void stop() {
    loop_.stop();
    // throw std::runtime_error("implement stop");
  }

  void close_session(server_session const &session) {
    LOG_INFO("blip");
    for (auto it = begin(sessions_); it != end(sessions_); ++it) {
      if (it->get() == &session) {
        LOG_DEBUG("found session to remove");
        sessions_.erase(it);
        LOG_DEBUG("Removed session.");
      }
    }
  }

  impl(server *parent, uint16_t port)
      : parent_(parent), suppress_exceptions_(false) {}

  uv_tcp_t socket_;
  uv_loop loop_;
  server *parent_;
  std::vector<std::unique_ptr<server_session>> sessions_;
  rpc::detail::thread_group loop_workers_;
  std::atomic_bool suppress_exceptions_;
  RPCLIB_CREATE_LOG_CHANNEL(server)
};

RPCLIB_CREATE_LOG_CHANNEL(server)

server::server(uint16_t port)
    : pimpl(new server::impl(this, port)),
      disp_(std::make_shared<dispatcher>()) {
  LOG_INFO("Created server on localhost:{}", port);
}

server::server(server &&other) noexcept {
  *this = std::move(other);
}

server::server(std::string const &address, uint16_t port)
    : pimpl(new server::impl(this, address, port)),
      disp_(std::make_shared<dispatcher>()) {
  LOG_INFO("Created server on address {}:{}", address, port);
}

server::~server() {
  if (pimpl) {
  }
}

server &server::operator=(server &&other) {
  pimpl = std::move(other.pimpl);
  other.pimpl = nullptr;
  disp_ = std::move(other.disp_);
  other.disp_ = nullptr;
  return *this;
}

void server::suppress_exceptions(bool suppress) {
  pimpl->suppress_exceptions_ = suppress;
}

void server::run() {
  pimpl->run();
}

void server::async_run(std::size_t worker_threads) {
  pimpl->loop_workers_.create_threads(worker_threads, [this]() {
    name_thread("server");
    LOG_INFO("Starting");
    pimpl->run();
    LOG_INFO("Exiting");
  });
}

void server::stop() {
  pimpl->stop();
}

void server::close_sessions() {
  // pimpl->close_sessions();
  throw std::runtime_error("implement close_sessions");
}

void server::close_session(server_session const &session) {
  pimpl->close_session(session);
}

}  // namespace rpc
