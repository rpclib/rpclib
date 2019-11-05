#pragma once

#include <string>
#include <string_view>
#include <optional>
#include <tuple>

namespace rpc {

template<typename... Args>
struct call_t {
  call_t(std::string_view method, std::optional<std::size_t> id, Args... args)
    : method_(method), id_(id), args_(std::make_tuple(args...)) {

  }
  std::string method_;
  std::tuple<Args...> args_;
  std::optional<std::size_t> id_;
};

}
