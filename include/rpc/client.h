#pragma once

#include <future>
#include <memory>

#include "nonstd/optional.hpp"

#include "rpc/config.h"
#include "rpc/detail/log.h"
#include "rpc/detail/pimpl.h"
#include "rpc/msgpack.hpp"

namespace rpc {

//! \brief Implements a client that connects to a msgpack-rpc server and is
//! able to call functions synchronously or asynchronously. This is the main
//! interfacing point for implementing client applications.
//!
//! Use this class to connect to msgpack-rpc servers and call their exposed
//! functions. This class supports calling functions synchronously and
//! asynchronously. When the client object is created, it initiates connecting
//! to the given server asynchronically and disconnects when it is destroyed.
#if __MINGW32__
class client {
#else
class EXPORT client {
#endif
public:
    //! \brief Constructs a client.
    //!
    //! When a client is constructed, it initiates a connection
    //! asynchronically. This means that it will not block while the connection
    //! is established. However, when the first call is performed, it *might*
    //! block if the connection was not already established.
    //!
    //! \param addr The address of the server to connect to. This might be an
    //! IP address or a host name, too.
    //! \param port The port on the server to connect to.
    client(std::string const &addr, uint16_t port);

    //! \cond DOXYGEN_SKIP
    client(client const &) = delete;
    //! \endcond

    //! \brief Destructor.
    //!
    //! During destruction, the connection to the server is gracefully closed.
    //! This means that any outstanding reads and writes are completed first.
    ~client();

    //! \brief Calls a function with the given name and arguments (if any).
    //!
    //! \param func_name The name of the function to call on the server.
    //! \param args A variable number of arguments to pass to the called
    //! function.
    //!
    //! \tparam Args The types of the arguments. Each type in this parameter
    //! pack have to be serializable by msgpack.
    //!
    //! \returns A RPCLIB_MSGPACK::object containing the result of the function (if
    //! any). To obtain a typed value, use the msgpack API.
    //!
    //! \throws rpc::rpc_error if the server responds with an error.
    template <typename... Args>
    RPCLIB_MSGPACK::object_handle call(std::string const &func_name, Args... args);

    //! \brief Calls a function asynchronously with the given name and
    //! arguments.
    //!
    //! A call is performed asynchronously in the context of the client, i.e.
    //! this is not to be confused with parallel execution on the server.
    //! This function differs from `call` in that it does not wait for the
    //! result of the function. Instead, it returns a std::future that
    //! can be used to retrieve the result later.
    //!
    //! \param func_name The name of the function to call.
    //! \param args The arguments to pass to the function.
    //!
    //! \tparam Args The types of the arguments.
    //!
    //! \returns A std::future, possibly holding a future result
    //! (which is a RPCLIB_MSGPACK::object).
    template <typename... Args>
    std::future<RPCLIB_MSGPACK::object_handle> async_call(std::string const &func_name, Args... args);

    //! \brief Sends a notification with the given name and arguments (if any).
    //!
    //! Notifications are a special kind of calls. They can be used to notify
    //! the server, while not expecting a response. In `rpclib` terminology,
    //! a notification is like an `async_call` without a return value.
    //!
    //! \param func_name The name of the notification to call.
    //! \param args The arguments to pass to the function.
    //! \tparam Args The types of the arguments.
    //!
    //! \note This function returns immediately (possibly before the
    //! notification is written to the socket).
    template <typename... Args>
    void send(std::string const &func_name, Args... args);

    //! \brief Returns the timeout setting of this client in milliseconds.
    //!
    //! The timeout is applied to synchronous calls. If the timeout expires
    //! without receiving a response from the server, rpc::timeout exception
    //! will be thrown.
    //!
    //! \note The timeout has no effect on async calls. For those,
    //! the preferred timeout mechanism remains using std::future.
    //!
    //! The default value for timeout is 5000ms (5 seconds).
    nonstd::optional<int64_t> get_timeout() const;

    //! \brief Sets the timeout for synchronous calls. For more information,
    //! see get_timeout().
    void set_timeout(int64_t value);

    //! \brief Clears the timeout for synchronous calls. For more information,
    //! see get_timeout().
    void clear_timeout();

    //! \brief Enum representing the connection states of the client.
    enum class connection_state { initial, connected, disconnected, reset };

    //! \brief Returns the current connection state.
    connection_state get_connection_state() const;

    //! \brief Waits for the completion of all ongoing calls.
    void wait_all_responses();

private:
    //! \brief Type of a promise holding a future response.
    using rsp_promise = std::promise<RPCLIB_MSGPACK::object_handle>;

    enum class request_type { call = 0, notification = 2 };

    void wait_conn();
    void post(std::shared_ptr<RPCLIB_MSGPACK::sbuffer> buffer, int idx,
              std::string const& func_name,
              std::shared_ptr<rsp_promise> p);
    void post(RPCLIB_MSGPACK::sbuffer *buffer);
    int get_next_call_idx();
    RPCLIB_NORETURN void throw_timeout(std::string const& func_name);

private:
    static constexpr double buffer_grow_factor = 1.8;
    RPCLIB_DECLARE_PIMPL()
};
}

#include "rpc/client.inl"