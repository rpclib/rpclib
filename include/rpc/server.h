#pragma once

#ifndef SERVER_H_S0HB5KXY
#define SERVER_H_S0HB5KXY

#include "rpc/config.h"
#include "rpc/msgpack.hpp"
#include "rpc/dispatcher.h"

#include "rpc/detail/pimpl.h"

namespace rpc {

namespace detail {
class server_session;
}

//! \brief Implements a msgpack-rpc server. This is the main interfacing
//! point with the library for creating servers.
//!
//! The server maintains a registry of function bindings that it uses to
//! dispatch calls. It also takes care of managing worker threads and TCP
//! connections.
//! The server does not start listening right after construction in order
//! to allow binding functions before that. Use the `run` or `async_run`
//! functions to start listening on the port.
//! This class is not copyable, but moveable.
class server {
public:
    //! \brief Constructs a server that listens on the localhost on the
    //! specified port.
    //!
    //! \param port The port number to listen on.
    explicit server(uint16_t port);

    //! \brief Move constructor. This is implemented by calling the
    //! move assignment operator.
    //!
    //! \param other The other instance to move from.
    server(server&& other) noexcept;

    //! \brief Constructs a server that listens on the specified address on
    //! the specified port.
    //!
    //! \param address The address to bind to. This only works if oee of your
    //! network adapaters control the given address.
    //! \param port The port number to listen on.
    server(std::string const &address, uint16_t port);

    //! \brief Destructor.
    //!
    //! When the server is destroyed, all ongoin sessions are closed gracefully.
    ~server();

    //! \brief Move assignment operator.
    //!
    //! \param other The other instance to move from.
    //! \return The result of the assignment.
    server& operator=(server&& other);

    //! \brief Starts the server loop. This is a blocking call.
    //!
    //! First and foremost, running the event loop causes the server to start
    //! listening on the specified port. Also, as connections are established
    //! and calls are made by clients, the server executes the calls as part
    //! of this call. This means that the handlers are executed on the thread
    //! that calls `run`. Reads and writes are initiated by this function
    //! internally as well.
    void run();

    //! \brief Starts the server loop on one or more threads. This is a
    //! non-blocking call.
    //!
    //! This function behaves similarly to `run`, except the event loop is
    //! optionally started on different threads. Effectively this sets up a
    //! worker thread pool for the server. Handlers will be executed on one
    //! of the threads.
    //!
    //! \param worker_threads The number of worker threads to start.
    void async_run(std::size_t worker_threads = 1);

    //! \brief Returns the port number this server is listening on.
    //! \note This is especially useful if server is started with an arbitrary
    //! port number (using port 0 in constructors).
    uint16_t get_port() const;

    //! \brief Binds a functor to a name so it becomes callable via RPC.
    //!
    //! This function template accepts a wide range of callables. The arguments
    //! and return types of these callables should be serializable by msgpack.
    //! `bind` effectively generates a suitable, light-weight compile-time
    //! wrapper for the functor.
    //!
    //! \param name The name of the functor.
    //! \param func The functor to bind.
    //! \tparam F The type of the functor.
    template <typename F> void bind(std::string const &name, F func) {
        disp_->bind(name, func);
    }

    //! \brief Sets the exception behavior in handlers. By default,
    //! handlers throwing will crash the server. If suppressing is on,
    //! the server will try to gather textual data and return it to
    //! the client as an error response.
    //! \note Setting this flag only affects subsequent connections.
    void suppress_exceptions(bool suppress);

    //! \brief Sets the behavior when using non-unique functor names in `bind`.
    //! By default, bind will throw. If overriding is on, old functor will be
    //! removed and replaced with new one.
    //! \note Setting this flag only affects subsequent functor bindings.
    void override_functors(bool override);

    //! \brief Stops the server.
    //! \note This should not be called from worker threads.
    void stop();

    //! \brief Closes all sessions gracefully.
    void close_sessions();

    friend class detail::server_session;

private:
    //! \brief Closes a specific session.
    void close_session(std::shared_ptr<detail::server_session> const& s);

private:
	RPCLIB_DECLARE_PIMPL()
    std::shared_ptr<detail::dispatcher> disp_;
};

} /* rpc */

#endif /* end of include guard: SERVER_H_S0HB5KXY */
