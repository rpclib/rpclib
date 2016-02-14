#pragma once

#ifndef DISPATCHER_H_CXIVZD5L
#define DISPATCHER_H_CXIVZD5L

#include <atomic>
#include <functional>
#include <memory>
#include <unordered_map>

#include "format.h"
#include <msgpack.hpp>

#include "callme/detail/call.h"
#include "callme/detail/func_tools.h"
#include "callme/detail/func_traits.h"
#include "callme/detail/log.h"
#include "callme/detail/not.h"
#include "callme/response.h"

namespace callme {

//! \brief This class maintains a registry of functors associated with their
//! names, and callable using a msgpack-rpc call pack.
class dispatcher {
public:
    //! \brief Binds a functor to a name so it becomes callable via RPC.
    //! \param name The name of the functor.
    //! \param func The functor to bind.
    //! \tparam F The type of the functor.
    //! \tparam Args The types of the arguments.
    template <typename F> void bind(std::string const &name, F func);

    //! \defgroup Tag-dispatched bind implementations for various functor cases.
    //! @{

    //! \brief Stores a void, zero-arg functor with a name.
    template <typename F>
    void bind(std::string const &name, F func,
              detail::tags::void_result const &,
              detail::tags::zero_arg const &);

    //! \brief Stores a void, non-zero-arg functor with a name.
    template <typename F>
    void bind(std::string const &name, F func,
              detail::tags::void_result const &,
              detail::tags::nonzero_arg const &);

    //! \brief Stores a non-void, zero-arg functor with a name.
    template <typename F>
    void bind(std::string const &name, F func,
              detail::tags::nonvoid_result const &,
              detail::tags::zero_arg const &);

    //! \brief Stores a non-void, non-zero-arg functor with a name.
    template <typename F>
    void bind(std::string const &name, F func,
              detail::tags::nonvoid_result const &,
              detail::tags::nonzero_arg const &);

    //! @}

    //! \brief Processes a message that contains a call according to
    //! the Msgpack-RPC spec.
    //! \param msg The buffer that contains the messagepack.
    //! \throws std::runtime_error If the messagepack does not contain a
    //! a call or the types of the parameters are not convertible to the called
    //! functions' parameters.
    void dispatch(msgpack::sbuffer const &msg);

    //! \brief Processes a message that contains a call according to
    //! the Msgpack-RPC spec.
    //! \param msg_obj The messagepack object that contains the call.
    //! \param suppress_exceptions If true, exceptions will be caught and
    //! written
    //! as response for the client.
    //! \throws std::runtime_error If the types of the parameters are not
    //! convertible to the called functions' parameters.
    response dispatch(msgpack::object const &msg,
                      bool suppress_exceptions = false);

    //! \brief This functor type unifies the interfaces of functions that are
    //!        called remotely
    using adaptor_type = std::function<std::unique_ptr<msgpack::object>(
        msgpack::object const &)>;

    //! \brief This is the type of messages as per the msgpack-rpc spec.
    using msg_type = std::tuple<int8_t, uint32_t, std::string, msgpack::object>;

private:
    std::unordered_map<std::string, adaptor_type> funcs_;
    CALLME_CREATE_LOG_CHANNEL(dispatcher)
};
}

#include "dispatcher.inl"

#endif /* end of include guard: DISPATCHER_H_CXIVZD5L */
