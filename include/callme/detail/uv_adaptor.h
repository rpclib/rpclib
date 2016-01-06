#pragma once

#ifndef UV_ADAPTOR_H_4N1PSMQL
#define UV_ADAPTOR_H_4N1PSMQL

#include "callme/detail/log.h"

namespace callme {
namespace detail {

//! \brief Adapts static callback to object member functions. Also provides
//! handle cleanup and
//! a loop member.
//! The object must may use the fw_on* functions as libuv callbacks,
//! as long as the following is true:
//! - The class has a function with the expected signature, without the fw_
//! prefix.
//! - The data members in the libuv callbacks are pointers to the object.
template <typename Adapted> class uv_adaptor {
protected:
    //! \defgroup Static callbacks that forward calls to members using
    //! the passed data pointer.
    //! @{
    //! \brief Forwards the on_new_connection callback to a member function.
    static void fw_on_new_connection(uv_stream_t *stream, int status) {
        auto obj = reinterpret_cast<Adapted *>(stream->data);
        obj->on_new_connection(stream, status);
    }

    //! \brief Forwards the on_read callback to a member function.
    static void fw_on_read(uv_stream_t *stream, ssize_t nread,
                           const uv_buf_t *buf) {
        auto obj = reinterpret_cast<Adapted *>(stream->data);
        obj->on_read(stream, nread, buf);
    }

    //! \brief Forwards the on_write callback to a member function.
    static void fw_on_write(uv_write_t *handle, int status) {
        auto obj = reinterpret_cast<Adapted *>(handle->data);
        obj->on_write(handle, status);
    }

    //! \brief Forwards the alloc_buffer callback to a member function.
    static void fw_alloc_buffer(uv_handle_t *handle, size_t size,
                                uv_buf_t *buffer) {
        auto obj = reinterpret_cast<Adapted *>(handle->data);
        obj->alloc_buffer(handle, size, buffer);
    }

    //! \brief Forwards the on_connect callback to a member function.
    static void fw_on_connect(uv_connect_t *handle, int status) {
        auto obj = reinterpret_cast<Adapted *>(handle->data);
        obj->on_connect(handle, status);
    }

    static void fw_on_close(uv_handle_t *handle) {
        auto obj = reinterpret_cast<Adapted *>(handle->data);
        obj->on_close(handle);
    }
    //! @}

protected:
    uv_loop_t loop_;
};
}
}

#endif /* end of include guard: UV_ADAPTOR_H_4N1PSMQL */
