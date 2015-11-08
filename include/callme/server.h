#pragma once

#ifndef CALLME_H_NBGBTUFL
#define CALLME_H_NBGBTUFL

#include <functional>
#include <memory>
#include <unordered_map>

#include <boost/utility/string_ref.hpp>
#include <msgpack.hpp>

namespace boost {
namespace asio {
class io_service;
}
}

namespace callme {

namespace detail {

// invoke a function with parameters from a tuple
// taken from http://stackoverflow.com/a/20441189/140367
template <typename R, template <typename...> class Params, typename... Args,
          std::size_t... I>
R invoke_helper(std::function<R(Args...)> const &func,
                Params<Args...> const &params, std::index_sequence<I...>) {
    return func(std::get<I>(params)...);
}

// "return func(params...)"
template <typename R, template <typename...> class Params, typename... Args>
R invoke(std::function<R(Args...)> const &func, Params<Args...> const &params) {
    return invoke_helper(func, params, std::index_sequence_for<Args...>{});
}
}

//! \brief Implements a MsgPack-RPC server.
class server {
public:
    //! \brief Constructs a MsgPack-RPC server.
    //! \param address The address to listen on. Can be "localhost", a IPv4 or
    //!        IPv6 address.
    //! \param port The port to listen on.
    server(boost::string_ref address, int16_t port);

    //! \brief Binds a function pointer to a name so becomes callable via RPC.
    //! \param name The name of the functor.
    //! \param func The functor to bind.
    //! \tparam R The return type of the functor.
    //! \tparam Args The types of the arguments.
    template <typename R, typename... Args>
    void bind(boost::string_ref name, R (*func)(Args...)) {
        bind(name, std::function<R(Args...)>(func));
    }

    //! \brief Binds a functor to a name so becomes callable via RPC.
    //! \param name The name of the functor.
    //! \param func The functor to bind.
    //! \tparam R The return type of the functor.
    //! \tparam Args The types of the arguments.
    template <typename R, typename... Args>
    void bind(boost::string_ref name, std::function<R(Args...)> func) {
        funcs_.insert(std::make_pair(
            name.to_string(),
            [func](msgpack::object const &args) -> msgpack::object {
                msgpack::type::tuple<Args...> args_real;
                args.convert(&args_real);
                callme::detail::invoke(func, args_real);
            }));
    }

    template <typename... Args>
    void bind(boost::string_ref name, std::function<void(Args...)> func) {
    }

    //! \brief This functor type unifies the interfaces of functions that are
    //!        called remotely
    typedef std::function<msgpack::object(msgpack::object const &)>
        adaptor_type;

private:
    std::shared_ptr<boost::asio::io_service> io_;
    std::unordered_map<std::string, adaptor_type> funcs_;
};
}

#include "server.inl"

#endif /* end of include guard: CALLME_H_NBGBTUFL */
