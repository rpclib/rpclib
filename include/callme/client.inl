namespace callme {

template <typename... Args>
msgpack::object client::call(std::string const &func_name, Args... args) {
    auto future = call_async(func_name, std::forward<Args>(args)...);
    future.wait();
    return future.get();
}

template <typename... Args>
std::future<msgpack::object> client::call_async(std::string const &func_name,
                                                Args... args) {
    using msgpack::object;
    LOG_DEBUG("Calling {}", func_name);
    wait_conn();

    auto args_obj = std::make_tuple(args...);
    const int idx = call_idx_++;
    auto call_obj =
        std::make_tuple(static_cast<uint8_t>(client::call_type::sync), idx,
                        func_name, args_obj);

    msgpack::sbuffer buffer;
    msgpack::pack(buffer, call_obj);

    ongoing_calls_.insert(std::make_pair(idx, std::promise<object>()));
    write_queue_.enqueue(std::move(buffer));

    return ongoing_calls_[idx].get_future();
}
}
