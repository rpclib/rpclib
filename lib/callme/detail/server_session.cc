#include "callme/detail/server_session.h"
#include "callme/detail/log.h"

namespace callme {
namespace detail {

server_session::server_session(asio::io_service *io,
                               asio::ip::tcp::socket socket,
                               std::shared_ptr<dispatcher> disp)
    : async_writer(io, std::move(socket)),
      io_(io),
      read_strand_(*io),
      disp_(disp),
      pac_() {
    pac_.reserve_buffer(default_buffer_size); // TODO: make this configurable
                                              // [sztomi 2016-01-13]
}

void server_session::start() { do_read(); }

void server_session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
        asio::buffer(pac_.buffer(), default_buffer_size),
        read_strand_.wrap([this, self](std::error_code ec, std::size_t length) {
            if (!ec) {
                pac_.buffer_consumed(length);
                msgpack::unpacked result;
                while (pac_.next(&result)) {
                    auto msg = result.get();
                    LOG_TRACE("msgpack read {}", msg);
                    output_buf_.clear();

                    // any worker thread can take this call
                    io_->post([
                        this, msg, z = std::shared_ptr<msgpack::zone>(
                                       result.zone().release())
                    ]() {
                        auto resp = disp_->dispatch(msg);
#ifdef _MSC_VER
                        std::function<void()> mary_had_a_little_lambda =
                            [this, resp, z]() { write(resp.get_data()); };
                        write_strand_.post(mary_had_a_little_lambda);
#else
                        write_strand_.post(
                            [this, resp, z]() { write(resp.get_data()); });
#endif
                    });
                }

                do_read();
            }
        }));
}

} /* detail */
} /* callme */
