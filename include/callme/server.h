#pragma once

#ifndef SERVER_H_S0HB5KXY
#define SERVER_H_S0HB5KXY

#include <stdint.h>
#include <memory>

#include "msgpack.hpp"
#include "uv.h"

#include "callme/string_ref.h"
#include "callme/dispatcher.h"

namespace callme {

class server {
public:
    explicit server(string_ref address, uint16_t port);

    //! \brief Starts the server loop. This is a blocking call.
    void run();

    //! \brief Binds a functor to a name so it becomes callable via RPC.
    //! \param name The name of the functor.
    //! \param func The functor to bind.
    //! \tparam F The type of the functor.
    template <typename F> void bind(string_ref name, F func) {
        disp_.bind(name, func);
    }

    void suppress_exceptions(bool suppress);

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


private:
    dispatcher disp_;
    uv_tcp_t tcp_;
    uv_loop_t *loop_;
    msgpack::unpacker pac_;
    bool suppress_exceptions_;
};

} /* callme */

#endif /* end of include guard: SERVER_H_S0HB5KXY */
