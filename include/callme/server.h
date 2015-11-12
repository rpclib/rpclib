#pragma once

#ifndef CALLME_H_NBGBTUFL
#define CALLME_H_NBGBTUFL

#include <iostream>
#include <functional>
#include <memory>
#include <unordered_map>

#include <boost/utility/string_ref.hpp>
#include <msgpack.hpp>

#include "detail/func_tools.h"

namespace boost {
namespace asio {
class io_service;
}
}

namespace callme {

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
    template <typename Functor,
              typename = detail::enable_if<std::is_void<
                  typename detail::func_traits<Functor>::result_type>>,
              typename = void>
    void bind(boost::string_ref name, Functor func) {
        using detail::func_traits;
        using tuple_type = typename func_traits<Functor>::args_type;
        using single_arg_type =
            typename func_traits<Functor>::template arg<0>::type;
        using args_type =
            typename std::conditional<(func_traits<Functor>::arg_count > 1),
                                      tuple_type, single_arg_type>::type;

        funcs_.insert(std::make_pair(
            name.to_string(), [func](msgpack::object const &args) {
                args_type args_real;
                args.convert(&args_real);
                detail::invoke2(func, tuple_type(args_real));
                return msgpack::object();
            }));
    }

    //! \brief Binds a function pointer to a name so becomes callable via RPC.
    //! \param name The name of the functor.
    //! \param func The functor to bind.
    //! \tparam R The return type of the functor.
    //! \tparam Args The types of the arguments.
    template <typename Functor,
              typename = detail::disable_if<std::is_void<
                  typename detail::func_traits<Functor>::result_type>>>
    void bind(boost::string_ref name, Functor func) {
        using detail::func_traits;
        using tuple_type = typename func_traits<Functor>::args_type;
        using single_arg_type =
            typename func_traits<Functor>::template arg<0>::type;
        using args_type =
            typename std::conditional<(func_traits<Functor>::arg_count > 1),
                                      tuple_type, single_arg_type>::type;

        funcs_.insert(std::make_pair(
            name.to_string(), [func](msgpack::object const &args) {
                args_type args_real;
                args.convert(&args_real);
                return msgpack::object(detail::invoke2(func, tuple_type(args_real)));
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
