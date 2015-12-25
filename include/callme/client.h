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
    //! \returns A callme::maybe object, possibly holding a result (which is a
    //! msgpack::object). \see callme::maybe.
    //! \tparam Args THe types of the arguments.
    template<typename... Args>
    maybe call_async(string_ref func_name, Args... args);

};

}

#include "callme/client.inl"
