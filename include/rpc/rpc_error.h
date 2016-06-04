#pragma once

#ifndef RPC_ERROR_H_NEOOSTKY
#define RPC_ERROR_H_NEOOSTKY

#include <exception>

#include "msgpack.hpp"

namespace rpc {
//! \brief Thrown when the server signals an error
//! during the call.
class rpc_error : public std::runtime_error {
public:
    //! \brief Returns the name of the function that was
    //! called on the server.
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
