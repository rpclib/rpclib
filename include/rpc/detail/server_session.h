#pragma once

#ifndef SESSION_H_5KG6ZMAB
#define SESSION_H_5KG6ZMAB

#include <atomic>
#include <memory>
#include <vector>

#include <uv.h>

#include "asio.hpp"

#include "rpc/config.h"
#include "rpc/msgpack.hpp"

#include "rpc/detail/async_writer.h"
#include "rpc/detail/log.h"
#include "rpc/detail/uv_adaptor.h"
#include "rpc/dispatcher.h"

namespace rpc {

class server;

namespace detail {

class server_session : public uv_adaptor<server_session> {
public:
  server_session(server *srv,
                 uv_loop_t *loop,
                 std::unique_ptr<uv_tcp_t> client_socket,
                 std::shared_ptr<dispatcher> disp,
                 bool suppress_exceptions);

  ~server_session();

  void start();

  void close();

private:
  void start_read();
  void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
  void on_write(uv_write_t *request, int status);
  void on_close(uv_handle_t *handle);
  void alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t *buffer);

private:
  server *parent_;
  uv_loop_t *loop_;
  std::unique_ptr<uv_tcp_t> client_socket_;
  std::shared_ptr<dispatcher> disp_;
  RPCLIB_MSGPACK::unpacker pac_;
  RPCLIB_MSGPACK::sbuffer output_buf_;
  const bool suppress_exceptions_;
  std::atomic_bool exit_;
  RPCLIB_CREATE_LOG_CHANNEL(session)

  friend class uv_adaptor<server_session>;
};
}  // namespace detail
}  // namespace rpc

#endif /* end of include guard: SESSION_H_5KG6ZMAB */
