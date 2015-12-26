namespace callme
{

template<typename... Args>
msgpack::object call(string_ref func_name, Args... args) {
    auto args_tuple = std::make_tuple(args...);
    auto call = pack_call(func_name, args_tuple);
}

template<typename... Args>
msgpack::object call_async(string_ref func_name, Args... args) {
    auto args_tuple = std::make_tuple(args...);
    auto call = pack_call(func_name, args_tuple);
}

template<typename... Args>
msgpack::object pack_call(string_ref name, std::tuple<Args...> const& args) {
}

}
