#pragma once

#include "callme/maybe.h"
#include "msgpack.hpp"
#include "callme/detail/string_ref.h"
#include "callme/detail/uv_adaptor.h"

namespace callme
{

//! \brief Implements a client that connects to a msgpack-rpc server and is
//! able to call functions synchronously or asynchronously.
class client : public detail::uv_adaptor<client>
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

    //! \brief Handles a new connection
    void on_new_connection(uv_stream_t *stream, int status);

    //! \brief Handles reading from a stream.
    void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

    //! \brief Allocates a buffer directly inside the unpacker, avoiding a copy.
    void alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t *buffer);

    friend class detail::uv_adaptor<client>;

private:
    uint16_t port_;
    std::string addr_;
};

}

#include "callme/client.inl"
