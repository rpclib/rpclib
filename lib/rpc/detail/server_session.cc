#include "rpc/detail/server_session.h"

#include "rpc/config.h"
#include "rpc/server.h"
#include "rpc/this_handler.h"
#include "rpc/this_server.h"
#include "rpc/this_session.h"

#include "backward.hpp"

#include "rpc/detail/log.h"
#include "rpc/detail/uv_tools.h"

namespace rpc {
namespace detail {

static constexpr std::size_t default_buffer_size =
    rpc::constants::DEFAULT_BUFFER_SIZE;

struct write_request {
  response rsp;
};

server_session::server_session(server *srv,
                               uv_loop_t *loop,
                               std::unique_ptr<uv_tcp_t> client_socket,
                               std::shared_ptr<dispatcher> disp,
                               bool suppress_exceptions)
    : parent_(srv),
      loop_(loop),
      client_socket_(std::move(client_socket)),
      disp_(disp),
      pac_(),
      output_buf_(),
      suppress_exceptions_(suppress_exceptions),
      exit_(false) {
  LOG_TRACE("this = {}", (void *)this);
  LOG_TRACE("parent = {}", (void *)parent_);
  client_socket_->data = this;
  pac_.reserve_buffer(default_buffer_size);  // TODO: make this configurable
                                             // [sztomi 2016-01-13]
  start();
}

server_session::~server_session() {
  LOG_INFO("Destroying session.");
}

void server_session::start() {
  start_read();
}

void server_session::close() {
  LOG_INFO("Closing session.");
  using namespace backward;
  StackTrace st;
  st.load_here(32);
  Printer p;
  p.object = true;
  p.color_mode = ColorMode::always;
  p.address = true;
  p.print(st, stderr);
  exit_ = true;
  uv_close(reinterpret_cast<uv_handle_t *>(client_socket_.get()), &fw_on_close);
}

void server_session::on_close(uv_handle_t *handle) {
  LOG_TRACE("on_close");
  LOG_TRACE("this = {}", (void *)this);
  LOG_TRACE("parent = {}", (void *)parent_);
  parent_->close_session(*this);
}

void server_session::start_read() {
  constexpr std::size_t max_read_bytes = default_buffer_size;
  uv_read_start(reinterpret_cast<uv_stream_t *>(client_socket_.get()),
                &fw_alloc_buffer, &fw_on_read);
  if (exit_) {
    // TODO: socket_.close(k);
    abort();
  }
}

void server_session::on_write(uv_write_t *request, int status) {
  if (status < 0) {
    throw std::runtime_error("on_write error bro");
  }

  auto write_req = reinterpret_cast<write_request *>(request->data);
  delete write_req;
}

void server_session::alloc_buffer(uv_handle_t *handle,
                                  size_t size,
                                  uv_buf_t *buffer) {
  (void)handle;
  pac_.reserve_buffer(size);
  buffer->base = pac_.buffer();
  buffer->len = size;
}

void server_session::on_read(uv_stream_t *stream,
                             ssize_t length,
                             const uv_buf_t *) {
  if (exit_) {
    return;
  }
  LOG_TRACE("Read of size {}", length);
  if (length == UV_EOF) {
    close();
    return;
  }
  check_uv("on_read length", length);
  pac_.buffer_consumed(static_cast<std::size_t>(length));
  RPCLIB_MSGPACK::unpacked result;
  while (pac_.next(result) && !exit_) {
    auto msg = result.get();
    output_buf_.clear();

    // any worker thread can take this call
    this_handler().clear();
    this_session().clear();
    this_session().set_id(reinterpret_cast<session_id_t>(this));
    this_server().cancel_stop();

    auto resp = disp_->dispatch(msg, suppress_exceptions_);

    // There are various things that decide what to send
    // as a response. They have a precedence.

    // First, if the response is disabled, that wins
    // So You Get Nothing, You Lose! Good Day Sir!
    if (!this_handler().resp_enabled_) {
      return;
    }

    // Second, if there is an error set, we send that
    // and only third, if there is a special response, we
    // use it
    if (!this_handler().error_.get().is_nil()) {
      LOG_WARN("There was an error set in the handler");
      resp.capture_error(this_handler().error_);
    } else if (!this_handler().resp_.get().is_nil()) {
      LOG_WARN(
          "There was a special result set in the "
          "handler");
      resp.capture_result(this_handler().resp_);
    }

    if (!resp.is_empty()) {
#ifdef _MSC_VER
      // doesn't compile otherwise.
      write_strand_.post([=]() { write(resp.get_data()); });
#else
      auto req = new uv_write_t{};
      std::memset(req, 0, sizeof(uv_write_t));
      // TODO: store resp in req->data
      req->data = new write_request{resp};
      uv_buf_t write_buf;
      write_buf.base = resp.get_data().data();
      write_buf.len = resp.get_data().size();
      uv_buf_t buffers[] = {write_buf};
      uv_write(req, reinterpret_cast<uv_stream_t *>(client_socket_.get()),
               buffers, 1, &fw_on_write);
      // write_strand_.post([this, self, resp, z]() { write(resp.get_data());
      // });
#endif
    }

    if (this_session().exit_) {
      LOG_WARN("Session exit requested from a handler.");
      // posting through the strand so this comes after
      // the previous write
      throw std::runtime_error("implement session exit");
      // write_strand_.post([this]() { exit_ = true; });
    }

    if (this_server().stopping_) {
      LOG_WARN("Server exit requested from a handler.");
      // posting through the strand so this comes after
      // the previous write
      throw std::runtime_error("implement server exit");
      // write_strand_.post([this]() { parent_->close_sessions(); });
    }
  }

  if (!exit_) {
    // resizing strategy: if the remaining buffer size is
    // less than the maximum bytes requested from asio,
    // then request max_read_bytes. This prompts the unpacker
    // to resize its buffer doubling its size
    // (https://github.com/msgpack/msgpack-c/issues/567#issuecomment-280810018)
    if (pac_.buffer_capacity() < default_buffer_size) {
      LOG_TRACE("Reserving extra buffer: {}", default_buffer_size);
      pac_.reserve_buffer(default_buffer_size);
    }
    start_read();
  }
  // if (ec == RPCLIB_ASIO::error::eof ||
  //           ec == RPCLIB_ASIO::error::connection_reset) {
  // LOG_INFO("Client disconnected");
  // self->close();
  // } else {
  //   LOG_ERROR("Unhandled error code: {} | '{}'", ec, ec.message());
  // }
}

}  // namespace detail
}  // namespace rpc
