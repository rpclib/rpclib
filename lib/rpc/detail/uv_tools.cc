#include "rpc/detail/uv_tools.h"

#include "format.h"

namespace rpc {
namespace detail {
uv_buf_t alloc_buffer(uv_handle_t *handle, unsigned suggested_size) {
  (void)handle;
  return uv_buf_init(new char[suggested_size], suggested_size);
}

void check_uv(std::string const &msg, int code) {
  if (code < 0) {
    throw rpc::detail::libuv_error(
        RPCLIB_FMT::format("Check failed: '{}'. Error ({}:{}): '{}'", msg, code,
                           uv_err_name(code), uv_strerror(code)));
  }
}

}  // namespace detail
}  // namespace rpc