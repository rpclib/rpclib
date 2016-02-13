#include "callme/detail/server_session.h"
#include "callme/detail/log.h"

namespace callme {
namespace detail {

server_session::server_session(asio::io_service *io,
                               asio::ip::tcp::socket socket,
                               std::shared_ptr<dispatcher> disp)
    : io_(io),
      socket_(std::move(socket)),
      write_strand_(*io),
      read_strand_(*io),
      disp_(disp),
      pac_() {
    pac_.reserve_buffer(default_buffer_size); // TODO: make this configurable
                                              // [sztomi 2016-01-13]
}

void server_session::start() { do_read(); }

void server_session::write(msgpack::sbuffer &&data) {
    write_queue_.push_back(std::move(data));
    if (write_queue_.size() > 1) {
        return; // there is an ongoing write chain so don't start another
    }

    do_write();
}

void server_session::do_write() {
    auto &item = write_queue_.front();
    // the data in item remains valid until the handler is called
    // since it will still be in the queue physically until then.
    asio::async_write(
        socket_, asio::buffer(item.data(), item.size()),
        write_strand_.wrap([this](std::error_code ec, std::size_t transferred) {
            write_queue_.pop_front();
            if (write_queue_.size() > 0) {
                do_write();
            }
        }));
}

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
                        io_->post(write_strand_.wrap(
                            [this, resp, z]() { write(resp.get_data()); }));
                    });
                }

                do_read();
            }
        }));
}

} /* detail */
} /* callme */
