//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2009 FURUHASHI Sadayuki
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
#ifndef MSGPACK_TYPE_NIL_HPP
#define MSGPACK_TYPE_NIL_HPP

#include "rpc/msgpack/versioning.hpp"
#include "rpc/msgpack/adaptor/adaptor_base.hpp"

namespace clmdep_msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace type {
    
    struct nil_t { };
    
    inline bool operator<(nil_t const& lhs, nil_t const& rhs) {
        return &lhs < &rhs;
    }
    
    inline bool operator==(nil_t const& lhs, nil_t const& rhs) {
        return &lhs == &rhs;
    }
    
    }  // namespace type
    
    namespace adaptor {
    
    template <>
    struct convert<type::nil_t> {
        clmdep_msgpack::object const& operator()(clmdep_msgpack::object const& o, type::nil_t&) const {
            if(o.type != clmdep_msgpack::type::NIL) { throw clmdep_msgpack::type_error(); }
            return o;
        }
    };
    
    template <>
    struct pack<type::nil_t> {
        template <typename Stream>
        clmdep_msgpack::packer<Stream>& operator()(clmdep_msgpack::packer<Stream>& o, const type::nil_t&) const {
            o.pack_nil();
            return o;
        }
    };
    
    template <>
    struct object<type::nil_t> {
        void operator()(clmdep_msgpack::object& o, type::nil_t) const {
            o.type = clmdep_msgpack::type::NIL;
        }
    };
    
    template <>
    struct object_with_zone<type::nil_t> {
        void operator()(clmdep_msgpack::object::with_zone& o, type::nil_t v) const {
            static_cast<clmdep_msgpack::object&>(o) << v;
        }
    };
    
    } // namespace adaptror
    
    template <>
    inline void clmdep_msgpack::object::as<void>() const
    {
        clmdep_msgpack::type::nil_t v;
        convert(v);
    }
    
    /// @cond
    }  // MSGPACK_API_VERSION_NAMESPACE(v1)
    /// @endcond
    
    }  // namespace clmdep_msgpack
    
    #endif // MSGPACK_TYPE_NIL_HPP
