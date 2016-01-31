#include "callme/client.h"
#include "callme/detail/dev_utils.h"
#include "callme/detail/log.h"
#include "callme/response.h"
#include <thread>

#include "format.h"

using asio::ip::tcp;
using namespace callme::detail;

namespace callme {

client::client(std::string const &addr, uint16_t port)
    : io_(),
      strand_(io_),
      socket_(io_),
      addr_(addr),
      port_(port),
      call_idx_(0),
      is_connected_(false) {

    tcp::resolver resolver(io_);
    auto endpoint_it = resolver.resolve({addr_, std::to_string(port_)});
    do_connect(endpoint_it);
    workers_.create_threads(1, [this]() {
        name_thread("client");
        LOG_INFO("Starting");
        io_.run();
        LOG_INFO("Exiting");
    });
}

client::~client() { io_.stop(); }

void client::do_connect(tcp::resolver::iterator endpoint_iterator) {
    asio::async_connect(
        socket_, endpoint_iterator,
        [this](std::error_code ec, tcp::resolver::iterator) {
            if (!ec) {
                std::unique_lock<std::mutex> lock(mut_connection_finished_);
                LOG_INFO("Client connected to {}:{}", addr_, port_);
                is_connected_ = true;
                conn_finished_.notify_all();
                do_read();
            } else {
                LOG_ERROR("Error during connect: {}", ec);
            }
        });
}

void client::do_read() {
    socket_.async_read_some(
        asio::buffer(pac_.buffer(), default_buffer_size),
        strand_.wrap([this](std::error_code ec, std::size_t length) {
            LOG_TRACE("Reading from tcp. nread = {}", length);

            if (!ec) {
                pac_.buffer_consumed(length);
                msgpack::unpacked result;
                while (pac_.next(&result)) {
                    auto r = response(result.get());
                    auto &promise = ongoing_calls_[r.get_id()];
                    // ongoing_calls_.erase(r.get_id());
                    try {
                        if (r.get_error().size() > 0) {
                            throw std::runtime_error(
                                fmt::format("callme: error during RPC call: {}",
                                            r.get_error()));
                        }
                        promise.set_value(r.get_result());
                    } catch (...) {
                        promise.set_exception(std::current_exception());
                    }
                }

                // if (ongoing_calls_.size() > 0) {
                do_read();
                //}
            }
        }));
}

void client::wait_conn() {
    if (!is_connected_) {
        std::unique_lock<std::mutex> lock(mut_connection_finished_);
        conn_finished_.wait(lock);
    }
}
}
