
namespace callme {

template <typename F> void dispatcher::bind(boost::string_ref name, F func) {
    bind(name, func, typename detail::func_kind_info<F>::result_kind(),
         typename detail::func_kind_info<F>::args_kind());
}

template <typename F>
void dispatcher::bind(boost::string_ref name, F func,
                      detail::tags::void_result const &,
                      detail::tags::zero_arg const &) {
    funcs_.insert(std::make_pair(name, [func](msgpack::object const &args) {
        func();
        return std::make_unique<msgpack::object>();
    }));
}

template <typename F>
void dispatcher::bind(boost::string_ref name, F func,
                      detail::tags::void_result const &,
                      detail::tags::nonzero_arg const &) {
    using detail::func_traits;
    using args_type = typename func_traits<F>::args_type;

    funcs_.insert(std::make_pair(name, [func, name](msgpack::object const &args) {
        args_type args_real;
        constexpr int args_count = std::tuple_size<args_type>::value;
        if (args.via.array.size != args_count) {
            throw std::runtime_error(
                fmt::format("Function '{0}' was called with an invalid number of "
                            "arguments. Expected: {1}, got: {2}",
                            name.to_string(), args_count, args.via.array.size));
        }
        args.convert(&args_real);
        detail::call(func, args_real);
        return std::make_unique<msgpack::object>();
    }));
}

template <typename F>
void dispatcher::bind(boost::string_ref name, F func,
                      detail::tags::nonvoid_result const &,
                      detail::tags::zero_arg const &) {
    using detail::func_traits;

    funcs_.insert(std::make_pair(name, [func](msgpack::object const &args) {
        return std::make_unique<msgpack::object>(func());
    }));
}

template <typename F>
void dispatcher::bind(boost::string_ref name, F func,
                      detail::tags::nonvoid_result const &,
                      detail::tags::nonzero_arg const &) {
    using detail::func_traits;
    using args_type = typename func_traits<F>::args_type;

    funcs_.insert(std::make_pair(name, [func, name](msgpack::object const &args) {
        constexpr int args_count = std::tuple_size<args_type>::value;
        if (args.via.array.size != args_count) {
            throw std::runtime_error(
                fmt::format("Function '{0}' was called with an invalid number of "
                            "arguments. Expected: {1}, got: {2}",
                            name.to_string(), args_count, args.via.array.size));
        }
        args_type args_real;
        args.convert(&args_real);
        return std::make_unique<msgpack::object>(detail::call(func, args_real));
    }));
}
}
