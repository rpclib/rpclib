#pragma once

#ifndef RESPONSE_H_MVRZEKPX
#define RESPONSE_H_MVRZEKPX

#include "msgpack.hpp"
#include "uv.h"

#include "callme/detail/uv_adaptor.h"

namespace callme {

//! \brief Represents a response and creates a msgpack to be sent back
//! as per the msgpack-rpc spec.
class response : public detail::uv_adaptor<response> {
public:
    //! \brief Constructs a msgpack::response with the given values.
    response(uint32_t id, std::string const &error,
             std::unique_ptr<msgpack::object> result);

    //! \brief Constructs a response from msgpack::object (useful when
    //! reading a response from a stream).
    response(msgpack::object const &o);

    //! \brief Writes response to the stream
    void write(uv_stream_t *stream);

    //! \brief Writes the response pack to a buffer.
    void write(msgpack::sbuffer *buf) const;

    //! \brief Returns the call id/index used to identify which call
    //! this response corresponds to.
    int get_id() const;

    //! \brief Returns the error message stored in the response. Can
    //! be empty.
    std::string const &get_error() const;

    //! \brief Returns the result stored in the response. Can be empty.
    msgpack::object get_result() const;

    //! \brief The type of a response, according to the msgpack-rpc spec
    using response_type =
        std::tuple<uint32_t, uint32_t, msgpack::object, msgpack::object>;

    friend class detail::uv_adaptor<response>;

private:
    void on_write(uv_write_t *req, int status);

private:
    uint32_t id_;
    std::string error_;
    std::unique_ptr<msgpack::object> result_;
};

} /* callme  */

#endif /* end of include guard: RESPONSE_H_MVRZEKPX */
