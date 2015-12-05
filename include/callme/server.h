#pragma once

#ifndef SERVER_H_S0HB5KXY
#define SERVER_H_S0HB5KXY

#include <stdint.h>
#include <atomic>
#include <memory>

#include <boost/utility/string_ref.hpp>

#include "msgpack.hpp"
#include "uv.h"
#include "callme/dispatcher.h"

namespace callme {

class server {
public:
    explicit server(boost::string_ref address, uint16_t port);

    void run();

    //! \brief Binds a functor to a name so it becomes callable via RPC.
    //! \param name The name of the functor.
    //! \param func The functor to bind.
    //! \tparam F The type of the functor.
    template <typename F> void bind(boost::string_ref name, F func) {
        disp_.bind(name, func);
    }

    //! \brief This enum describes various strategies for handling exceptions
    //! in the server that come from user code.
    enum class exception_strategy {
        //! \brief The server tries to extract some information from the
        //! exception, sends a response with it to the client and
        //! rethrows it. This is the default behavior.
        response_rethrow = 1,
        //! \brief The server does not handle exceptions in any way,
        //! it is the responsibility of the user. May lead to losing
        //! responses in the client.
        no_action = 2,
        //! \brief The server tries to extract some information from the
        //! exception, sends a response with it to the client and
        //! does not rethrow it.
        response_only = 4
    };

    //! \brief Sets the exception handling strategy of the server.
    //! \param The strategy to use.
    //! \see exception_strategy
    void set_exception_strategy(exception_strategy s);

    //! \brief Returns the currently active exception handling strategy of the
    //! server.
    //! \see exception_strategy
    exception_strategy get_exception_strategy() const;

private:
    //! \defgroup Static callbacks that forward calls to members using
    //! the passed data pointer.
    //! @{
    //! \brief Forwards the on_new_connection callback to a member function.
    static void fw_on_new_connection(uv_stream_t *srv, int status);

    //! \brief Forwards the on_read callback to a member function.
    static void fw_on_read(uv_stream_t *stream, ssize_t nread,
                           const uv_buf_t *buf);

    //! \brief Forwards the alloc_buffer callback to a member function.
    static void fw_alloc_buffer(uv_handle_t *handle, size_t size,
                                uv_buf_t *buffer);
    //! @}

    //! \brief Handles a new connection
    void on_new_connection(uv_stream_t *stream, int status);

    //! \brief Handles reading from a stream.
    void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

    //! \brief Allocates a buffer directly inside the unpacker, avoiding a copy.
    void alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t *buffer);

public:
    dispatcher disp_; // TODO: temporarily public [sztomi, 2015-11-21]

private:
    uv_tcp_t tcp_;
    uv_loop_t *loop_;
    msgpack::unpacker pac_;
    std::atomic<exception_strategy> exc_strat_;
};

} /* callme */

#endif /* end of include guard: SERVER_H_S0HB5KXY */
