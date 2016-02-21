#pragma once

#ifndef OBJECT_H_8WIFSU0W
#define OBJECT_H_8WIFSU0W

#include <tuple>
#include "msgpack.hpp"

namespace callme {
namespace detail {

class zones {
public:
    static zones& instance() {
        static zones inst;
        return inst;
    }

    msgpack::zone& server() { return server_; }
    msgpack::zone& client() { return client_; }

private:
    zones() = default;

    msgpack::zone server_, client_;
};

struct object {
    msgpack::object o;
    msgpack::zone z;
};
} /* detail */

} /* callme */

#endif /* end of include guard: OBJECT_H_8WIFSU0W */
