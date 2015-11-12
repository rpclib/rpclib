#pragma once

#ifndef FUNC_TOOLS_H_9FNRD4G2
#define FUNC_TOOLS_H_9FNRD4G2

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

template <typename R, template <typename...> class Params, typename... Args>
R invoke(std::function<R(Args...)> const &func, Params<Args...> const &params) {
    return invoke_helper(func, params, std::index_sequence_for<Args...>{});
}

template <int N, typename... Ts>
using nth_type = typename std::tuple_element<N, std::tuple<Ts...>>::type;

template <typename T>
struct func_traits : public func_traits<decltype(&T::operator())> {};

template <typename R, typename... Args> struct func_traits<R (*)(Args...)> {
    using result_type = R;
    using args_type = std::tuple<Args...>;
    static const int arg_count = sizeof...(Args);

    template <std::size_t i> struct arg {
        using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
    };
};

template <typename C, typename R, typename... Args>
struct func_traits<R (C::*)(Args...) const> {
    using result_type = R;
    using args_type = std::tuple<Args...>;
    static const int arg_count = sizeof...(Args);

    template <std::size_t i> struct arg {
        using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
    };
};

template <typename Functor, typename... Args, std::size_t... I>
decltype(auto) invoke2_helper(Functor func, std::tuple<Args...> &&params,
                              std::index_sequence<I...>) {
    return func(std::get<I>(params)...);
}

template <typename Functor, typename... Args>
decltype(auto) invoke2(Functor f, std::tuple<Args...> &&args) {
    return invoke2_helper(f, std::forward<std::tuple<Args...>>(args),
                          std::index_sequence_for<Args...>{});
}

template <typename Condition, typename T = void>
using enable_if = typename std::enable_if<Condition::value, T>::type;

template <typename Condition, typename T = void>
using disable_if = typename std::enable_if<!Condition::value, T>::type;

}
}

#endif /* end of include guard: FUNC_TOOLS_H_9FNRD4G2 */
