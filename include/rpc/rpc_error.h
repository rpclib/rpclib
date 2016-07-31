#pragma once

#ifndef RPC_ERROR_H_NEOOSTKY
#define RPC_ERROR_H_NEOOSTKY

#include <exception>

#include "msgpack.hpp"

namespace rpc {

//! \brief This exception is thrown by the client when the server signals an
//! error during a call.
//!
//! It type allows clients to handle arbitrary error objects as the
//! msgpack-rpc specification allows. In client code you probably don't want to
//! throw it, hence its constructor is private.
class rpc_error : public std::runtime_error {
public:
    //! \brief Returns the name of the function that was
    //! called on the server while the error occurred.
    std::string get_function_name() const;

    //! \brief Returns the error object that the server
    //! provided.
    msgpack::object_handle& get_error();

private:
    friend class client;
    rpc_error(std::string const &what_arg, std::string const &function_name,
              msgpack::object_handle o);

private:
    std::string func_name_;
    msgpack::object_handle ob_h_;
};
} /* rpc */

#endif /* end of include guard: RPC_ERROR_H_NEOOSTKY */
