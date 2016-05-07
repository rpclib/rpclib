#pragma once

#ifndef RPC_ERROR_H_NEOOSTKY
#define RPC_ERROR_H_NEOOSTKY

#include <exception>

#include "msgpack.hpp"

namespace callme {
//! \brief Thrown when the server signals an error
//! during the call.
class rpc_error : public std::runtime_error {
public:
    //! \brief Returns the name of the function that was
    //! called on the server.
    std::string get_function_name() const;

    //! \brief Returns the error object that the server
    //! provided.
    std::shared_ptr<msgpack::object_handle> get_error() const;

private:
    friend class client;
    rpc_error(std::string const &what_arg, std::string const &function_name,
              std::shared_ptr<msgpack::object_handle> o);

private:
    std::string func_name_;
    std::shared_ptr<msgpack::object_handle> ob_h_;
};
} /* callme */

#endif /* end of include guard: RPC_ERROR_H_NEOOSTKY */
