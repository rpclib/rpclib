#pragma once

#ifndef UTIL_H_YRIZ63UJ
#define UTIL_H_YRIZ63UJ

#include "msgpack.hpp"

namespace callme {
namespace detail {
template <typename T> msgpack::object_handle pack(T &&o) {
    auto z = std::make_unique<msgpack::zone>();
    msgpack::object o(std::forward<T>(o), *z);
    return msgpack::object_handle(o, std::move(z));
}
} /* detail */
} /* callme  */

#endif /* end of include guard: UTIL_H_YRIZ63UJ */
