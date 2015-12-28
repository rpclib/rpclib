namespace callme
{

template<typename... Args>
msgpack::object client::call(string_ref func_name, Args... args) {
    using msgpack::object;
    auto args_obj = object(std::make_tuple(args...));
    auto call_obj = object(std::make_tuple(client::call_type::sync, 
                call_idx_++, func_name, args_obj));
}

template<typename... Args>
maybe client::call_async(string_ref func_name, Args... args) {
    auto args_tuple = std::make_tuple(args...);
}

}
