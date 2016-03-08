#pragma once

#ifndef ASYNC_WRITER_H_HQIRH28I
#define ASYNC_WRITER_H_HQIRH28I

#include "asio.hpp"
#include "msgpack.hpp"
#include <deque>

namespace callme {

class client;

namespace detail {

//! \brief Common logic for classes that have a write queue with async writing.
class async_writer {
public:
    async_writer(CALLME_ASIO::io_service *io,
                 CALLME_ASIO::ip::tcp::socket socket)
        : socket_(std::move(socket)), write_strand_(*io), exit_(false) {}

    void do_write() {
        auto &item = write_queue_.front();
        // the data in item remains valid until the handler is called
        // since it will still be in the queue physically until then.
        CALLME_ASIO::async_write(
            socket_, CALLME_ASIO::buffer(item.data(), item.size()),
            write_strand_.wrap(
                [this](std::error_code ec, std::size_t transferred) {
                    (void)transferred;
                    if (!ec) {
                        write_queue_.pop_front();
                        if (write_queue_.size() > 0) {
                            if (!exit_) {
                                do_write();
                            }
                        }
                    } else {
                        LOG_ERROR("Error while writing to socket: {}", ec);
                    }
                }));
    }

    void write(msgpack::sbuffer &&data) {
        write_queue_.push_back(std::move(data));
        if (write_queue_.size() > 1) {
            return; // there is an ongoing write chain so don't start another
        }

        do_write();
    }

    friend class callme::client;

protected:
    CALLME_ASIO::ip::tcp::socket socket_;
    CALLME_ASIO::strand write_strand_;
    std::atomic_bool exit_;

private:
    std::deque<msgpack::sbuffer> write_queue_;
};

} /* detail */
} /* callme  */

#endif /* end of include guard: ASYNC_WRITER_H_HQIRH28I */
