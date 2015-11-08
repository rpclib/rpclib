#pragma once

#ifndef CALLME_H_NBGBTUFL
#define CALLME_H_NBGBTUFL

#include <iostream>
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
template <typename R, typename Arg>
R invoke(std::function<R(Arg)> const &func, Arg &arg) {
    return func(arg);
}

template <typename R, template <typename...> class Params, typename... Args>
R invoke(std::function<R(Args...)> const &func, Params<Args...> const &params) {
    return invoke_helper(func, params, std::index_sequence_for<Args...>{});
}

template <int N, typename... Ts>
using nth_type = typename std::tuple_element<N, std::tuple<Ts...>>::type;
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

    //! \brief Binds a non-void functor to a name so becomes callable via RPC.
    //! \param name The name of the functor.
    //! \param func The functor to bind.
    //! \tparam R The return type of the functor.
    //! \tparam Args The types of the arguments.
    template <typename R, typename... Args>
    void bind(boost::string_ref name, std::function<R(Args...)> func) {
        funcs_.insert(std::make_pair(
            name.to_string(),
            [func](msgpack::object const &args) -> msgpack::object {
                std::tuple<Args...> args_real;
                args.convert(&args_real);
                return msgpack::object(callme::detail::invoke(func, args_real));
            }));
    }

    //! \brief Binds a void functor to a name so becomes callable via RPC.
    //! \param name The name of the functor.
    //! \param func The functor to bind.
    //! \tparam Args The types of the arguments.
    template <typename... Args>
    void bind(boost::string_ref name, std::function<void(Args...)> func) {
        adaptor_type functor;
        using tuple_type = std::tuple<Args...>;
        using single_arg_type = detail::nth_type<0, Args...>;
        using args_type =
            typename std::conditional<(sizeof...(Args) > 1), tuple_type,
                                      single_arg_type>::type;

        funcs_.insert(std::make_pair(
            name.to_string(),
            [func](msgpack::object const &args) -> msgpack::object {
                args_type args_real;
                args.convert(&args_real);
                detail::invoke(func, args_real);
                return msgpack::object();
            }));
    }

    //! \brief Processes a message that contains a call according to
    //! the Msgpack-RPC spec.
    //! \param msg The buffer that contains the messagepack.
    //! \throws std::runtime_error If the messagepack does not contain a
    //! a call or the types of the parameters are not convertible to the called
    //! functions' parameters.
    void process_call(msgpack::sbuffer const &msg);

    //! \brief This functor type unifies the interfaces of functions that are
    //!        called remotely
    using adaptor_type =
        std::function<msgpack::object(msgpack::object const &)>;

    //! \brief This is the type of messages as per the msgpack-rpc spec.
    using msg_type = std::tuple<int8_t, uint32_t, std::string, msgpack::object>;

private:
    std::shared_ptr<boost::asio::io_service> io_;
    std::unordered_map<std::string, adaptor_type> funcs_;
};
}

#include "server.inl"

#endif /* end of include guard: CALLME_H_NBGBTUFL */
