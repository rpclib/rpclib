namespace callme
{

template<typename... Args>
msgpack::object client::call(string_ref func_name, Args... args) {
    using msgpack::object;
    auto args_obj = std::make_tuple(args...);
    auto call_obj =
        std::make_tuple(static_cast<uint8_t>(client::call_type::sync),
                        call_idx_++, func_name.to_string(), args_obj);
    msgpack::pack(buf_, call_obj);

    uv_write_t request;
    uv_buf_t uvbuf;
    uvbuf.base = buf_.data();
    uvbuf.len = buf_.size();
    uv_write(&request, reinterpret_cast<uv_stream_t *>(&tcp_), &uvbuf, 1,
             &client::fw_on_write);
}

template<typename... Args>
maybe client::call_async(string_ref func_name, Args... args) {
    auto args_tuple = std::make_tuple(args...);
}

}
