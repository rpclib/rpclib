#include "callme/detail/server_session.h"
#include "callme/detail/log.h"
#include "callme/this_handler.h"

namespace callme {
namespace detail {

server_session::server_session(CALLME_ASIO::io_service *io,
                               CALLME_ASIO::ip::tcp::socket socket,
                               std::shared_ptr<dispatcher> disp,
                               bool suppress_exceptions)
    : async_writer(io, std::move(socket)),
      io_(io),
      read_strand_(*io),
      disp_(disp),
      pac_(),
      suppress_exceptions_(suppress_exceptions) {
    pac_.reserve_buffer(default_buffer_size); // TODO: make this configurable
                                              // [sztomi 2016-01-13]
}

void server_session::start() { do_read(); }

void server_session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
        CALLME_ASIO::buffer(pac_.buffer(), default_buffer_size),
        read_strand_.wrap([this, self](std::error_code ec, std::size_t length) {
            if (!ec) {
                pac_.buffer_consumed(length);
                msgpack::unpacked result;
                while (pac_.next(&result)) {
                    auto msg = result.get();
                    output_buf_.clear();

                    // any worker thread can take this call
                    io_->post([
                        this, msg, z = std::shared_ptr<msgpack::zone>(
                                       result.zone().release())
                    ]() {
                        this_handler.clear();
                        auto resp = disp_->dispatch(msg, suppress_exceptions_);

                        // There are various things that decide what to send
                        // as a response. They have a precedence.
                        
                        // First, if the response is disabled, that wins
                        // So You Get Nothing, You Lose! Good Day Sir! 
                        if (!this_handler.resp_enabled_) {
                            return;
                        }

                        // Second, if there is an error set, we send that
                        if (!this_handler.error_.get().is_nil()) {
                            resp.set_error(this_handler.error_); // TODO: upgrade set_error to take object_handles
                        } 
                        else if (!this_handler.resp_.get().is_nil()) {
                            resp.set_result(this_handler.resp_); // TODO
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
                    });
                }

                do_read();
            }
        }));
}

} /* detail */
} /* callme */
