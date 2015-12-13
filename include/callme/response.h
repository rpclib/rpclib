#pragma once

#ifndef RESPONSE_H_MVRZEKPX
#define RESPONSE_H_MVRZEKPX

#include "msgpack.hpp"
#include "uv.h"

#include "callme/string_ref.h"

namespace callme {

//! \brief Represents a response and creates a msgpack to be sent back
//! as per the msgpack-rpc spec.
class response {
public:
    response(uint32_t id, string_ref error,
             std::unique_ptr<msgpack::object> result);

    //! \brief Writes response to the stream
    void write(uv_stream_t *stream);

    //! \brief The type of a response, according to the msgpack-rpc spec
    using response_type =
        std::tuple<uint32_t, uint32_t, msgpack::object, msgpack::object&>;

private:
    uint32_t id_;
    std::string error_;
    std::unique_ptr<msgpack::object> result_;
};

} /* callme  */

#endif /* end of include guard: RESPONSE_H_MVRZEKPX */
