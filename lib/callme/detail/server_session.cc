#include "callme/detail/server_session.h"
#include "callme/detail/log.h"

namespace callme {
namespace detail {

server_session::server_session(asio::ip::tcp::socket socket, std::shared_ptr<dispatcher> disp)
    : socket_(std::move(socket)), disp_(disp), pac_() {
    pac_.reserve_buffer(default_buffer_size); // TODO: make this configurable
                                              // [sztomi 2016-01-13]
}

void server_session::start() { do_read(); }

void server_session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
        asio::buffer(pac_.buffer(), default_buffer_size),
        [this, self](std::error_code ec, std::size_t length) {
            if (!ec) {
                pac_.buffer_consumed(length);
                msgpack::unpacked result;
                while (pac_.next(&result)) {
                    auto msg = result.get();
                    LOG_DEBUG("msgpack read from tcp.");
                    buf_.clear();
                    try {
                        auto resp = disp_->dispatch(msg);
                        resp.write(&buf_);
                    } catch (std::exception &e) {
                        dispatcher::msg_type the_call;
                        msg.convert(&the_call);
                        // TODO: This is a bit redundant because the dispatcher
                        // does this, too. There is a chance that we have to
                        // fuse the dispatcher and server classes
                        // [t.szelei 2015-12-09]
                        auto &&id = std::get<1>(the_call);
                        auto &&name = std::get<2>(the_call);
                        auto &&args = std::get<3>(the_call);

                        response error_resp(
                            id,
                            fmt::format(
                                "callme: function '{0}' (taking {1} arg(s)) "
                                "threw an exception. The exception "
                                "contained this information: {2}.",
                                name, args.via.array.size, e.what()),
                            std::make_unique<msgpack::object>());
                        error_resp.write(&buf_);

                        //if (!suppress_exceptions_) {
                            throw;
                        //}
                    }

                    asio::async_write(socket_,
                                      asio::buffer(buf_.data(), buf_.size()),
                                      [this, self](std::error_code ec,
                                                   std::size_t /* length */) {
                                          if (!ec) {
                                              do_read();
                                          } else {
                                              // TODO: Handle error [sztomi
                                              // 2016-01-14]
                                          }
                                      });
                }

                do_read();
            }
        });
}

} /* detail */
} /* callme */
