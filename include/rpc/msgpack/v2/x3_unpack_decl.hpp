//
// MessagePack for C++ deserializing routine
//
// Copyright (C) 2018 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGPACK_V2_X3_UNPACK_DECL_HPP
#define MSGPACK_V2_X3_UNPACK_DECL_HPP

#if defined(MSGPACK_USE_X3_PARSE)

#include "rpc/msgpack/versioning.hpp"

namespace clmdep_msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v2) {
/// @endcond

namespace detail {

template <typename Iterator>
void
unpack_imp(Iterator&& begin, Iterator&& end,
           clmdep_msgpack::zone& result_zone, clmdep_msgpack::object& result, bool& referenced,
           unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR,
           unpack_limit const& limit = unpack_limit());

} // namespace detail

template <typename Iterator>
clmdep_msgpack::object_handle unpack(
    Iterator&& begin, Iterator&& end,
    bool& referenced,
    unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR,
    unpack_limit const& limit = unpack_limit());

template <typename Iterator>
clmdep_msgpack::object_handle unpack(
    Iterator&& begin, Iterator&& end,
    unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR,
    unpack_limit const& limit = unpack_limit());

template <typename Iterator>
clmdep_msgpack::object unpack(
    clmdep_msgpack::zone& z,
    Iterator&& begin, Iterator&& end,
    bool& referenced,
    unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR,
    unpack_limit const& limit = unpack_limit());

template <typename Iterator>
clmdep_msgpack::object unpack(
    clmdep_msgpack::zone& z,
    Iterator&& begin, Iterator&& end,
    unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR,
    unpack_limit const& limit = unpack_limit());

/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v2)
/// @endcond

}  // namespace clmdep_msgpack


#endif // defined(MSGPACK_USE_X3_PARSE)

#endif // MSGPACK_V2_X3_UNPACK_DECL_HPP
