#pragma once

#ifndef SERVER_H_S0HB5KXY
#define SERVER_H_S0HB5KXY

#include <stdint.h>
#include <memory>

#include "asio.hpp"
#include "msgpack.hpp"
#include "callme/dispatcher.h"

namespace callme {

class server {
public:
    explicit server(std::string const& address, uint16_t port);

    //! \brief Starts the server loop. This is a blocking call.
    void run();

    void start();

    //! \brief Binds a functor to a name so it becomes callable via RPC.
    //! \param name The name of the functor.
    //! \param func The functor to bind.
    //! \tparam F The type of the functor.
    template <typename F> void bind(std::string const& name, F func) {
        disp_->bind(name, func);
    }

    void suppress_exceptions(bool suppress);

private:
    asio::io_service io_;
    asio::ip::tcp::acceptor acceptor_;
    asio::ip::tcp::socket socket_;
    std::shared_ptr<dispatcher> disp_;
    bool suppress_exceptions_;
};

} /* callme */

#endif /* end of include guard: SERVER_H_S0HB5KXY */
