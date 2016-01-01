#pragma once

#ifndef SERVER_H_S0HB5KXY
#define SERVER_H_S0HB5KXY

#include <stdint.h>
#include <memory>

#include "msgpack.hpp"
#include "uv.h"

#include "callme/string_ref.h"
#include "callme/dispatcher.h"
#include "callme/detail/uv_adaptor.h"

namespace callme {

class server : public detail::uv_adaptor<server> {
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
    //! \brief Handles a new connection
    void on_new_connection(uv_stream_t *stream, int status);

    //! \brief Handles reading from a stream.
    void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

    //! \brief Allocates a buffer directly inside the unpacker, avoiding a copy.
    void alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t *buffer);

    friend class uv_adaptor<server>;


private:
    dispatcher disp_;
    uv_tcp_t tcp_;
    uv_loop_t *loop_;
    msgpack::unpacker pac_;
    bool suppress_exceptions_;
};

} /* callme */

#endif /* end of include guard: SERVER_H_S0HB5KXY */
