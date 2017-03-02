#pragma once

#ifndef CALL_H_ZXFACADH
#define CALL_H_ZXFACADH

#include <tuple>
#include "rpc/detail/func_tools.h"
#include "rpc/detail/invoke.h"
#include "rpc/detail/is_specialization_of.h"

namespace rpc {
namespace detail {


//! \brief Calls a functor with argument provided directly
template <typename Functor, typename Arg>
auto call(Functor f, Arg &&arg)
    -> decltype(f(std::forward<Arg>(arg))) {
    return f(std::forward<Arg>(arg));
}

#if RPCLIB_CXX_STANDARD == 14
template <typename Functor, typename... Args, std::size_t... I>
auto call_helper(Functor func, std::tuple<Args...> &&params,
        std::index_sequence<I...>)
    -> decltype(func(std::get<I>(params)...)) {
    return func(std::get<I>(params)...);
}

//! \brief Calls a functor with arguments provided as a tuple
template <typename Functor, typename... Args>
auto call(Functor f, std::tuple<Args...> &args)
    -> decltype(call_helper(f, std::forward<std::tuple<Args...>>(args), std::index_sequence_for<Args...>{})) {
    return call_helper(f, std::forward<std::tuple<Args...>>(args),
                       std::index_sequence_for<Args...>{});
}

#elif RPCLIB_CXX_STANDARD == 11

// We're limited to 5 arguments per call
template <typename Functor, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
auto call(Functor f, std::tuple<Arg0, Arg1, Arg2, Arg3, Arg4> &args)
    -> decltype(f(std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args), std::get<4>(args)))
{
    return f(
            std::get<0>(args),
            std::get<1>(args),
            std::get<2>(args),
            std::get<3>(args),
            std::get<4>(args)
    );
}

template <typename Functor, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
auto call(Functor f, std::tuple<Arg0, Arg1, Arg2, Arg3> &args)
    -> decltype(f(std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args)))
{
    return f(
            std::get<0>(args),
            std::get<1>(args),
            std::get<2>(args),
            std::get<3>(args)
    );
}

template <typename Functor, typename Arg0, typename Arg1, typename Arg2>
auto call(Functor f, std::tuple<Arg0, Arg1, Arg2> &args)
    -> decltype(f(std::get<0>(args), std::get<1>(args), std::get<2>(args)))
{
    return f(
            std::get<0>(args),
            std::get<1>(args),
            std::get<2>(args)
    );
}

template <typename Functor, typename Arg0, typename Arg1>
auto call(Functor f, std::tuple<Arg0, Arg1> &args)
    -> decltype(f(std::get<0>(args), std::get<1>(args)))
{
    return f(
            std::get<0>(args),
            std::get<1>(args)
    );
}

template <typename Functor, typename Arg0>
auto call(Functor f, std::tuple<Arg0> &args)
    -> decltype(f(std::get<0>(args)))
{
    return f(
            std::get<0>(args)
    );
}

#endif

}
}

#endif /* end of include guard: CALL_H_ZXFACADH */
