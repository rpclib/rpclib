#pragma once

#include <future>

#include "callme/detail/log.h"
#include "callme/detail/pimpl.h"
#include "msgpack.hpp"

namespace callme {

//! \brief Implements a client that connects to a msgpack-rpc server and is
//! able to call functions synchronously or asynchronously.
class client {
public:
    client(std::string const &addr, uint16_t port);

    client(client const &) = delete;

    ~client();

    //! \brief Calls a function with the given name and arguments (if any).
    //! \param func_name The name of the function to call.
    //! \param args The arguments to pass to the function.
    //! \returns A msgpack::object containing the result of the function (if
    //! any).
    //! To obtain a typed value, use the msgpack API.
    //! \tparam Args THe types of the arguments.
    template <typename... Args>
    msgpack::object_handle call(std::string const &func_name, Args... args);

    //! \brief Calls a function asynchronously with the given name and
    //! arguments.
    //! \param func_name The name of the function to call.
    //! \param args The arguments to pass to the function.
    //! \returns A std::future, possibly holding a future result
    //! (which is a msgpack::object).
    //! \tparam Args The types of the arguments.
    template <typename... Args>
    std::future<msgpack::object_handle> async_call(std::string const &func_name,
                                                   Args... args);

    //! \brief Sends a notification with the given name and arguments (if any).
    //! \param func_name The name of the notification to call.
    //! \param args The arguments to pass to the function.
    //! \note This function returns immediately (possibly before the
    //! notification
    //! is written to the socket.
    //! \tparam Args THe types of the arguments.
    template <typename... Args>
    void send(std::string const &func_name, Args... args);

    //! \brief Enum representing the connection states of the client.
    enum class connection_state { initial, connected, disconnected, reset };

    //! \brief Returns the current connection state.
    connection_state get_connection_state() const;

    //! \brief Waits for the completion of all ongoing calls.
    void wait_all_responses();

private:
    enum class request_type { call = 0, notification = 2 };

    void wait_conn();
    void post(std::shared_ptr<msgpack::sbuffer> buffer, int idx,
              std::shared_ptr<std::promise<msgpack::object_handle>> p);
    void post(msgpack::sbuffer *buffer);
    int get_next_call_idx();

private:
    static const uint32_t default_buffer_size = 4096;
    CALLME_DECL_PIMPL(648)
};
}

#include "callme/client.inl"
