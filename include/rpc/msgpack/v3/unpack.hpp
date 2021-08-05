//
// MessagePack for C++ deserializing routine
//
// Copyright (C) 2018 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGPACK_V3_UNPACK_HPP
#define MSGPACK_V3_UNPACK_HPP

#include "rpc/msgpack/unpack_decl.hpp"
#include "rpc/msgpack/parse.hpp"
#include "rpc/msgpack/create_object_visitor.hpp"

namespace clmdep_msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v3) {
/// @endcond

inline clmdep_msgpack::object_handle unpack(
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    clmdep_msgpack::unpack_reference_func f, void* user_data,
    clmdep_msgpack::unpack_limit const& limit
)
{
    clmdep_msgpack::object obj;
    clmdep_msgpack::unique_ptr<clmdep_msgpack::zone> z(new clmdep_msgpack::zone);
    referenced = false;
    parse_return ret = detail::unpack_imp(
        data, len, off, *z, obj, referenced, f, user_data, limit);

    switch(ret) {
    case PARSE_SUCCESS:
        return clmdep_msgpack::object_handle(obj, clmdep_msgpack::move(z));
    case PARSE_EXTRA_BYTES:
        return clmdep_msgpack::object_handle(obj, clmdep_msgpack::move(z));
    default:
        break;
    }
    return clmdep_msgpack::object_handle();
}

inline clmdep_msgpack::object_handle unpack(
    const char* data, std::size_t len, std::size_t& off,
    clmdep_msgpack::unpack_reference_func f, void* user_data,
    clmdep_msgpack::unpack_limit const& limit)
{
    bool referenced;
    return clmdep_msgpack::v3::unpack(data, len, off, referenced, f, user_data, limit);
}

inline clmdep_msgpack::object_handle unpack(
    const char* data, std::size_t len, bool& referenced,
    clmdep_msgpack::unpack_reference_func f, void* user_data,
    clmdep_msgpack::unpack_limit const& limit)
{
    std::size_t off = 0;
    return clmdep_msgpack::v3::unpack(data, len, off, referenced, f, user_data, limit);
}

inline clmdep_msgpack::object_handle unpack(
    const char* data, std::size_t len,
    clmdep_msgpack::unpack_reference_func f, void* user_data,
    clmdep_msgpack::unpack_limit const& limit)
{
    bool referenced;
    std::size_t off = 0;
    return clmdep_msgpack::v3::unpack(data, len, off, referenced, f, user_data, limit);
}

inline void unpack(
    clmdep_msgpack::object_handle& result,
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    clmdep_msgpack::unpack_reference_func f, void* user_data,
    clmdep_msgpack::unpack_limit const& limit)
{
    clmdep_msgpack::object obj;
    clmdep_msgpack::unique_ptr<clmdep_msgpack::zone> z(new clmdep_msgpack::zone);
    referenced = false;
    parse_return ret = detail::unpack_imp(
        data, len, off, *z, obj, referenced, f, user_data, limit);

    switch(ret) {
    case PARSE_SUCCESS:
        result.set(obj);
        result.zone() = clmdep_msgpack::move(z);
        return;
    case PARSE_EXTRA_BYTES:
        result.set(obj);
        result.zone() = clmdep_msgpack::move(z);
        return;
    default:
        return;
    }
}

inline void unpack(
    clmdep_msgpack::object_handle& result,
    const char* data, std::size_t len, std::size_t& off,
    clmdep_msgpack::v3::unpack_reference_func f, void* user_data,
            clmdep_msgpack::unpack_limit const& limit)
{
    bool referenced;
    clmdep_msgpack::v3::unpack(result, data, len, off, referenced, f, user_data, limit);
}

inline void unpack(
    clmdep_msgpack::object_handle& result,
    const char* data, std::size_t len, bool& referenced,
    clmdep_msgpack::unpack_reference_func f, void* user_data,
    clmdep_msgpack::unpack_limit const& limit)
{
    std::size_t off = 0;
    clmdep_msgpack::v3::unpack(result, data, len, off, referenced, f, user_data, limit);
}

inline void unpack(
    clmdep_msgpack::object_handle& result,
    const char* data, std::size_t len,
    clmdep_msgpack::unpack_reference_func f, void* user_data,
    clmdep_msgpack::unpack_limit const& limit)
{
    bool referenced;
    std::size_t off = 0;
    clmdep_msgpack::v3::unpack(result, data, len, off, referenced, f, user_data, limit);
}


inline clmdep_msgpack::object unpack(
    clmdep_msgpack::zone& z,
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    clmdep_msgpack::unpack_reference_func f, void* user_data,
    clmdep_msgpack::unpack_limit const& limit)
{
    clmdep_msgpack::object obj;
    referenced = false;
    parse_return ret = detail::unpack_imp(
        data, len, off, z, obj, referenced, f, user_data, limit);

    switch(ret) {
    case PARSE_SUCCESS:
        return obj;
    case PARSE_EXTRA_BYTES:
        return obj;
    default:
        break;
    }
    return obj;
}

inline clmdep_msgpack::object unpack(
    clmdep_msgpack::zone& z,
    const char* data, std::size_t len, std::size_t& off,
    clmdep_msgpack::unpack_reference_func f, void* user_data,
    clmdep_msgpack::unpack_limit const& limit)
{
    bool referenced;
    return clmdep_msgpack::v3::unpack(z, data, len, off, referenced, f, user_data, limit);
}

inline clmdep_msgpack::object unpack(
    clmdep_msgpack::zone& z,
    const char* data, std::size_t len, bool& referenced,
    clmdep_msgpack::unpack_reference_func f, void* user_data,
    clmdep_msgpack::unpack_limit const& limit)
{
    std::size_t off = 0;
    return clmdep_msgpack::v3::unpack(z, data, len, off, referenced, f, user_data, limit);
}

inline clmdep_msgpack::object unpack(
    clmdep_msgpack::zone& z,
    const char* data, std::size_t len,
    clmdep_msgpack::unpack_reference_func f, void* user_data,
    clmdep_msgpack::unpack_limit const& limit)
{
    bool referenced;
    std::size_t off = 0;
    return clmdep_msgpack::v3::unpack(z, data, len, off, referenced, f, user_data, limit);
}

/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v3)
/// @endcond

}  // namespace clmdep_msgpack


#endif // MSGPACK_V3_UNPACK_HPP
