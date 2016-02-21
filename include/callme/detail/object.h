#pragma once

#ifndef OBJECT_H_8WIFSU0W
#define OBJECT_H_8WIFSU0W

#include <tuple>
#include "msgpack.hpp"

namespace callme {
namespace detail {

struct object {
    msgpack::object o;
    msgpack::zone z;
};
} /* detail */

} /* callme */

#endif /* end of include guard: OBJECT_H_8WIFSU0W */
