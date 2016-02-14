namespace callme {

template <typename... Args>
msgpack::object client::call(std::string const &func_name, Args... args) {
    auto future = async_call(func_name, std::forward<Args>(args)...);
    future.wait();
    return future.get();
}

template <typename... Args>
std::future<msgpack::object> client::async_call(std::string const &func_name,
                                                Args... args) {
    wait_conn();
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

    // So I think the following warrants a little explanation.
    // ongoing_calls_ can only be touched inside the strand. However, I need 
    // to return a future. In order to get the promise inside the lambda, I 
    // would normally move it, but right now asio::post does not accept 
    // handlers that are not copy constructable. Hence, I allocate the promise
    // on the heap, get its future, *copy the pointer inside the lambda*,
    // move the pointed promise into ongoing_calls_ and free whatever needs being
    // freed after the moved-from promise object pointed by p. 
    // Ugly, but works.
    // TODO: Change to plain moving when asio starts supporting move-only
    // handlers. [sztomi, 2016-02-14]
    auto p = new std::promise<object>();
    auto ft = p->get_future();

    strand_.post([this, buffer, idx, p]() {
                ongoing_calls_.insert(std::make_pair(idx, std::move(*p)));
                delete p;
                write(std::unique_ptr<msgpack::sbuffer>(buffer));
            });

    return ft;
}
}
