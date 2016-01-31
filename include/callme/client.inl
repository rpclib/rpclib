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
    wait_conn();
    using msgpack::object;

    LOG_DEBUG("Calling {}", func_name);

    auto args_obj = std::make_tuple(args...);
    const int idx = call_idx_++;
    auto call_obj =
        std::make_tuple(static_cast<uint8_t>(client::call_type::sync), idx,
                        func_name, args_obj);
    buf_.clear();
    msgpack::pack(buf_, call_obj);

    ongoing_calls_.insert(std::make_pair(idx, std::promise<object>()));

    asio::async_write(socket_, asio::buffer(buf_.data(), buf_.size()),
                      strand_.wrap([this](std::error_code ec, std::size_t) {
                          if (ec) {
                              LOG_ERROR("Error during write: {}", ec);
                              // TODO: Handle error [sztomi, 2016-01-14]
                          }
                      }));
    auto future = ongoing_calls_[idx].get_future();
    return future;
}
}
