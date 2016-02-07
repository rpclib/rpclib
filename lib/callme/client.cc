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
      exiting_(false),
      is_connected_(false) {

    tcp::resolver resolver(io_);
    auto endpoint_it = resolver.resolve({addr_, std::to_string(port_)});
    do_connect(endpoint_it);
    std::thread io_thread([this]() {
        name_thread("client");
        LOG_INFO("Starting");
        io_.run();
        LOG_INFO("Exiting");
    });
    io_thread_ = std::move(io_thread);
}

client::~client() {
    exiting_ = true;
    io_.stop();
    if (write_running_.test_and_set(std::memory_order_acquire)) {
        write_thread_.join();
    }
    io_thread_.join();
}

void client::write(std::unique_ptr<msgpack::sbuffer> item) {
    wait_conn();
    write_queue_.push_back(std::move(item));

    if (write_queue_.size() > 1) {
        return; // there is an ongoing write chain so don't start another
    }

    do_write();
}

void client::do_write() {
    auto &item = write_queue_.front();
    // the data in item remains valid until the handler is called
    // since it will still be in the queue physically until then.
    asio::async_write(
        socket_, asio::buffer(item->data(), item->size()),
        strand_.wrap([this](std::error_code ec, std::size_t transferred) {
            write_queue_.pop_front();
            if (write_queue_.size() > 0) {
                do_write();
            }
        }));
}

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
        [this](std::error_code ec, std::size_t length) {
            LOG_TRACE("Reading from tcp. nread = {}", length);

            if (!ec) {
                pac_.buffer_consumed(length);
                msgpack::unpacked result;
                while (pac_.next(&result)) {
                    auto r = response(result.get());
                    auto id = r.get_id();
                    auto &promise = ongoing_calls_[id];
                    strand_.post([this, id]() { ongoing_calls_.erase(id); });
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
        });
}

void client::wait_conn() {
    if (!is_connected_) {
        std::unique_lock<std::mutex> lock(mut_connection_finished_);
        conn_finished_.wait(lock);
    }
}
}
