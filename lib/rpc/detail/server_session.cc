#include "rpc/detail/server_session.h"
#include "rpc/detail/log.h"
#include "rpc/server.h"
#include "rpc/this_handler.h"
#include "rpc/this_server.h"
#include "rpc/this_session.h"

namespace rpc {
namespace detail {

server_session::server_session(server *srv, RPCLIB_ASIO::io_service *io,
                               RPCLIB_ASIO::ip::tcp::socket socket,
                               std::shared_ptr<dispatcher> disp,
                               bool suppress_exceptions)
    : async_writer(io, std::move(socket)),
      parent_(srv),
      io_(io),
      read_strand_(*io),
      disp_(disp),
      pac_(),
      suppress_exceptions_(suppress_exceptions) {
    pac_.reserve_buffer(default_buffer_size); // TODO: make this configurable
                                              // [sztomi 2016-01-13]
}

void server_session::start() { do_read(); }

void server_session::close() {
    LOG_INFO("Closing session.");
    exit_ = true;
    write_strand_.post([this]() { socket_.close(); });
}

void server_session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
        RPCLIB_ASIO::buffer(pac_.buffer(), default_buffer_size),
        read_strand_.wrap([this, self](std::error_code ec, std::size_t length) {
            if (!ec) {
                pac_.buffer_consumed(length);
                RPCLIB_MSGPACK::unpacked result;
                while (pac_.next(&result) && !exit_) {
                    auto msg = result.get();
                    output_buf_.clear();

                    // any worker thread can take this call
                    io_->post([
                        this, msg, z = std::shared_ptr<RPCLIB_MSGPACK::zone>(
                                       result.zone().release())
                    ]() {
                        this_handler().clear();
                        this_session().clear();
                        this_server().cancel_stop();

                        auto resp = disp_->dispatch(msg, suppress_exceptions_);

                        // There are various things that decide what to send
                        // as a response. They have a precedence.

                        // First, if the response is disabled, that wins
                        // So You Get Nothing, You Lose! Good Day Sir!
                        if (!this_handler().resp_enabled_) {
                            return;
                        }

                        // Second, if there is an error set, we send that
                        // and only third, if there is a special response, we
                        // use it
                        if (!this_handler().error_.get().is_nil()) {
                            LOG_WARN("There was an error set in the handler");
                            resp.capture_error(this_handler().error_);
                        } else if (!this_handler().resp_.get().is_nil()) {
                            LOG_WARN("There was a special result set in the "
                                     "handler");
                            resp.capture_result(this_handler().resp_);
                        }

                        if (!resp.is_empty()) {
#ifdef _MSC_VER
                            // doesn't compile otherwise.
                            write_strand_.post(
                                [=]() { write(resp.get_data()); });
#else
                            write_strand_.post(
                                [this, resp, z]() { write(resp.get_data()); });
#endif
                        }

                        if (this_session().exit_) {
                            LOG_WARN("Session exit requested from a handler.");
                            // posting through the strand so this comes after
                            // the previous write
                            write_strand_.post([this]() { exit_ = true; });
                        }

                        if (this_server().stopping_) {
                            LOG_WARN("Server exit requested from a handler.");
                            // posting through the strand so this comes after
                            // the previous write
                            write_strand_.post(
                                [this]() { parent_->close_sessions(); });
                        }
                    });
                }

                if (!exit_) {
                    do_read();
                }
            }
        }));
    if (exit_) {
        socket_.close();
    }
}

} /* detail */
} /* rpc */
