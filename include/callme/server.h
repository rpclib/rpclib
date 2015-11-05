#pragma once

#ifndef CALLME_H_NBGBTUFL
#define CALLME_H_NBGBTUFL

#include <functional>
#include <memory>

#include <boost/utility/string_ref.hpp>

namespace boost {
namespace asio {
    class io_service;
}
}

namespace callme {

//! \brief Implements a MsgPack-RPC server.
class server {
public:
    //! \brief Binds a functor to a name so becomes callable via RPC.
    //! \param name The name of the functor.
    //! \param func The functor to bind.
    //! \tparam R The return type of the functor.
    //! \tparam Args The types of the arguments.
    template<typename R, typename Args>
    void bind(boost::string_ref name, std::function<R (Args...)> func);

private:
    std::shared_ptr<boost::asio::io_service> service_;
};
}

#include "server.inl"

#endif /* end of include guard: CALLME_H_NBGBTUFL */
