#include "rpc/rpc_error.h"

namespace rpc {

rpc_error::rpc_error(std::string const &what_arg,
                     std::string const &function_name,
                     msgpack::object_handle o)
    : std::runtime_error(what_arg),
      func_name_(function_name),
      ob_h_(std::move(o)) {}

std::string rpc_error::get_function_name() const { return func_name_; }

msgpack::object_handle& rpc_error::get_error() {
    return ob_h_;
}

} /* rpc */
