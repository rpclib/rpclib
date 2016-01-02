#pragma once

#ifndef TESTUTILS_H_LHCAMVUX
#define TESTUTILS_H_LHCAMVUX

#include <tuple>
#include "msgpack.hpp"

namespace callme {
namespace testutils {

template <typename... Types> inline msgpack::unpacked make_obj(Types... items) {
    auto obj = std::make_tuple(items...);
    msgpack::sbuffer buf;
    msgpack::pack(buf, obj);
    msgpack::unpacked msg;
    msgpack::unpack(&msg, buf.data(), buf.size());
	return msg;
}
}
}

#endif /* end of include guard: TESTUTILS_H_LHCAMVUX */
