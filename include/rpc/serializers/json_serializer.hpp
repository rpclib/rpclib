#pragma once

#include "rpc/config.hpp"

#ifndef RPCLIB_HAVE_RAPIDJSON
#error "rpclib was configured without rapidjson."
#endif

#include <string>
#include <string_view>

#include <boost/hana/for_each.hpp>
#include <boost/hana/functional/overload.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/ext/std/tuple.hpp>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "rpc/call.hpp"

namespace rpc {

struct json_serializer {
  template <typename... Args>
  static std::string call_to_str(std::string_view fn, Args... args) {
    namespace hana = boost::hana;
    namespace rj = rapidjson;

    rj::StringBuffer buf;
    rj::Writer<rj::StringBuffer> writer(buf);
    writer.StartObject();

    writer.Key("jsonrpc");
    writer.String("2.0");

    writer.Key("method");
    writer.String(fn.data());

    writer.Key("params");
    writer.StartArray();

    auto write_arg =
        hana::overload([&writer](int x) { writer.Int(x); },
                       [&writer](int64_t x) { writer.Int64(x); },
                       [&writer](double x) { writer.Double(x); },
                       [&writer](unsigned int x) { writer.Uint(x); },
                       [&writer](uint64_t x) { writer.Uint64(x); },
                       [&writer](bool x) { writer.Bool(x); },
                       [&writer](const char *x) { writer.String(x); });

    hana::for_each(hana::make_tuple(args...), write_arg);

    writer.EndArray();
    writer.EndObject();

    return buf.GetString();
  }


  template <typename... Args>
  static std::string call_to_str(call_t<Args...> const& call) {
    namespace hana = boost::hana;
    namespace rj = rapidjson;

    rj::StringBuffer buf;
    rj::Writer<rj::StringBuffer> writer(buf);
    writer.StartObject();

    writer.Key("jsonrpc");
    writer.String("2.0");

    writer.Key("method");
    writer.String(call.method_.c_str());

    if (call.id_) {
      writer.Key("id");
      writer.Int64(*call.id_);
    }

    writer.Key("params");
    writer.StartArray();

    auto write_arg =
        hana::overload([&writer](int x) { writer.Int(x); },
                       [&writer](int64_t x) { writer.Int64(x); },
                       [&writer](double x) { writer.Double(x); },
                       [&writer](unsigned int x) { writer.Uint(x); },
                       [&writer](uint64_t x) { writer.Uint64(x); },
                       [&writer](bool x) { writer.Bool(x); },
                       [&writer](const char *x) { writer.String(x); });

    hana::for_each(call.args_, write_arg);

    writer.EndArray();
    writer.EndObject();

    return buf.GetString();
  }
};

} // namespace rpc
