#pragma once

#ifndef RPCLIB_UV_TOOLS_H_
#define RPCLIB_UV_TOOLS_H_

#include <stdexcept>

#include <uv.h>

uv_buf_t alloc_buffer(uv_handle_t *handle, unsigned suggested_size);

namespace rpc {
namespace detail {
struct libuv_error : std::runtime_error {
  explicit libuv_error(std::string const &e) : std::runtime_error(e) {}
};

void check_uv(std::string const &msg, int code);

}  // namespace detail
}  // namespace rpc

#endif