namespace callme {

template <typename... Args>
msgpack::object_handle client::call(std::string const &func_name,
                                    Args... args) {
    auto future = async_call(func_name, std::forward<Args>(args)...);
    future.wait();
    return future.get();
}

template <typename... Args>
std::future<msgpack::object_handle>
client::async_call(std::string const &func_name, Args... args) {
    wait_conn();
    using msgpack::object;
    LOG_DEBUG("Calling {}", func_name);

    auto args_obj = std::make_tuple(args...);
    const int idx = get_next_call_idx();
    auto call_obj =
        std::make_tuple(static_cast<uint8_t>(client::request_type::call), idx,
                        func_name, args_obj);

    auto buffer = new msgpack::sbuffer;
    msgpack::pack(*buffer, call_obj);

    // So I think the following warrants a little explanation. (see post())
    // ongoing_calls_ can only be touched inside the strand. However, I need
    // to return a future. In order to get the promise inside the lambda, I
    // would normally move it, but right now asio::post does not accept
    // handlers that are not copy constructable. Hence, I allocate the promise
    // on the heap, get its future, *copy the pointer inside the lambda*,
    // move the pointed promise into ongoing_calls_ and free whatever needs
    // being freed after the moved-from promise object pointed by p.
    // Ugly, but works. (same deal with buffer).
    // TODO: Change to plain moving when asio starts supporting move-only
    // handlers. [sztomi, 2016-02-14]
    auto p = new std::promise<msgpack::object_handle>();
    auto ft = p->get_future();

    post(buffer, idx, p);

    return ft;
}

//! \brief Sends a notification with the given name and arguments (if any).
//! \param func_name The name of the notification to call.
//! \param args The arguments to pass to the function.
//! \note This function returns when the notification is written to the
//! socket.
//! \tparam Args THe types of the arguments.
template <typename... Args>
void client::send(std::string const &func_name, Args... args) {
    LOG_DEBUG("Sending notification {}", func_name);

    auto args_obj = std::make_tuple(args...);
    auto call_obj = std::make_tuple(
        static_cast<uint8_t>(client::request_type::notification), func_name,
        args_obj);

    auto buffer = new msgpack::sbuffer;
    msgpack::pack(*buffer, call_obj);

    post(buffer);
}
}
