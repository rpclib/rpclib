namespace callme {

template <typename... Args>
msgpack::object client::call(std::string const& func_name, Args... args) {
    using msgpack::object;

    LOG_DEBUG("Calling %v", func_name);

    auto args_obj = std::make_tuple(args...);
    const int idx = call_idx_++;
    auto call_obj =
        std::make_tuple(static_cast<uint8_t>(client::call_type::sync), idx,
                        func_name, args_obj);
    msgpack::pack(buf_, call_obj);

    uv_write_t request;
    uv_buf_t uvbuf;
    uvbuf.base = buf_.data();
    uvbuf.len = buf_.size();

    ongoing_calls_.insert(std::make_pair(idx, std::promise<object>()));

    uv_write(&request, reinterpret_cast<uv_stream_t *>(&tcp_), &uvbuf, 1,
             &client::fw_on_write);

    auto future = ongoing_calls_[idx].get_future();
    future.wait();
    return future.get();
}

template <typename... Args>
maybe client::call_async(std::string const& func_name, Args... args) {
    using msgpack::object;
    auto args_tuple = std::make_tuple(args...);
    return maybe();
}
}
