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

    auto buffer = new msgpack::sbuffer;
    msgpack::pack(*buffer, call_obj);

    strand_.post([this, buffer, idx]() {
                ongoing_calls_.insert(std::make_pair(idx, std::promise<object>()));
                write(std::unique_ptr<msgpack::sbuffer>(buffer));
            });

    return ongoing_calls_[idx].get_future();
}
}
