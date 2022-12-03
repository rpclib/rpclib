//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2018 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGPACK_V1_TYPE_BYTE_HPP
#define MSGPACK_V1_TYPE_BYTE_HPP

#if __cplusplus >= 201703

#include "rpc/msgpack/versioning.hpp"
#include "rpc/msgpack/adaptor/adaptor_base.hpp"
#include "rpc/msgpack/adaptor/adaptor_base.hpp"
#include "rpc/msgpack/adaptor/int_decl.hpp"
#include "rpc/msgpack/object.hpp"

#include <cstddef>

namespace clmdep_msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <>
struct convert<std::byte> {
    clmdep_msgpack::object const& operator()(clmdep_msgpack::object const& o, std::byte& v) const {
        v = static_cast<std::byte>(type::detail::convert_integer<unsigned char>(o));
        return o;
    }
};

template <>
struct pack<std::byte> {
    template <typename Stream>
    clmdep_msgpack::packer<Stream>& operator()(clmdep_msgpack::packer<Stream>& o, std::byte v) const {
        o.pack_unsigned_char(static_cast<unsigned char>(v));
        return o;
    }
};

template <>
struct object<std::byte> {
    void operator()(clmdep_msgpack::object& o, std::byte v) const {
        o.type = clmdep_msgpack::type::POSITIVE_INTEGER;
        o.via.u64 = static_cast<unsigned char>(v);
    }
};

template <>
struct object_with_zone<std::byte> {
    void operator()(clmdep_msgpack::object::with_zone& o, const std::byte& v) const {
        static_cast<clmdep_msgpack::object&>(o) << v;
    }
};


} // namespace adaptor

/// @cond
} // MSGPACK_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace clmdep_msgpack

#endif // __cplusplus >= 201703

#endif // MSGPACK_V1_TYPE_BYTE_HPP
