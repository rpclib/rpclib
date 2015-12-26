#pragma once

#include "callme/detail/string_ref.h"
#include "callme/maybe.h"
#include "msgpack.hpp"

namespace callme
{

//! \brief Implements a client that connects to a msgpack-rpc server and is
//! able to call functions synchronously or asynchronously.
class client
{
public:
    client(string_ref addr, uint16_t port);

    //! \brief Calls a function with the given name and arguments (if any).
    //! \param func_name The name of the function to call.
    //! \param args The arguments to pass to the function.
    //! \returns A msgpack::object containing the result of the function (if any).
    //! To obtain a typed value, use the msgpack API.
    //! \tparam Args THe types of the arguments.
    template<typename... Args>
    msgpack::object call(string_ref func_name, Args... args);

    //! \brief Calls a function asynchronously with the given name and arguments.
    //! \param func_name The name of the function to call.
    //! \param args The arguments to pass to the function.
    //! \returns A callme::maybe object, possibly holding a future result
    //! (which is a msgpack::object). \see callme::maybe.
    //! \tparam Args THe types of the arguments.
    template<typename... Args>
    maybe call_async(string_ref func_name, Args... args);

private:
    template<typename... Args>
    msgpack::object pack_call(string_ref name, std::tuple<Args...> const& args);

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
};

}

#include "callme/client.inl"
