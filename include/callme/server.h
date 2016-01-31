#pragma once

#ifndef SERVER_H_S0HB5KXY
#define SERVER_H_S0HB5KXY

#include <stdint.h>
#include <memory>
#include <thread>

#include "asio.hpp"
#include "msgpack.hpp"
#include "callme/dispatcher.h"
#include "callme/detail/log.h"

namespace callme {

class server {
public:
    explicit server(std::string const &address, uint16_t port);
    ~server();

    //! \brief Starts the server loop. This is a blocking call.
    void run();

    //! \brief Starts the server loop on a separate thread. 
    //! This is a non-blocking call.
    void async_run();

    //! \brief Binds a functor to a name so it becomes callable via RPC.
    //! \param name The name of the functor.
    //! \param func The functor to bind.
    //! \tparam F The type of the functor.
    template <typename F> void bind(std::string const &name, F func) {
        disp_->bind(name, func);
    }

    void suppress_exceptions(bool suppress);

private:
    void start_accept();

private:
    asio::io_service io_;
    asio::ip::tcp::acceptor acceptor_;
    asio::ip::tcp::socket socket_;
    std::shared_ptr<dispatcher> disp_;
    std::unique_ptr<std::thread> loop_thread_;
    bool suppress_exceptions_;

    CALLME_CREATE_LOG_CHANNEL(server)
};

} /* callme */

#endif /* end of include guard: SERVER_H_S0HB5KXY */
