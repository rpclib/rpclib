namespace callme
{

template<typename... Args>
msgpack::object call(string_ref func_name, Args... args) {
    using msgpack::object;
    auto args_obj = object(std::make_tuple(args...));
    auto call_obj = object(std::make_tuple(call_type::sync, 
                call_idx_++, func_name, args_obj));
}

template<typename... Args>
msgpack::object call_async(string_ref func_name, Args... args) {
    auto args_tuple = std::make_tuple(args...);
    auto call = pack_call(func_name, args_tuple);
}

}
