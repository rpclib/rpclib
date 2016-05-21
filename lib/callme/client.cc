#include "callme/client.h"
#include "callme/rpc_error.h"

#include <atomic>
#include <condition_variable>
#include <deque>
#include <future>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "asio.hpp"
#include "format.h"

#include "callme/detail/async_writer.h"
#include "callme/detail/dev_utils.h"
#include "callme/detail/response.h"

using namespace CALLME_ASIO;
using CALLME_ASIO::ip::tcp;
using namespace callme::detail;

namespace callme {

struct client::impl {
    impl(client *parent, std::string const &addr, uint16_t port)
        : parent_(parent),
          io_(),
          strand_(io_),
          call_idx_(0),
          addr_(addr),
          port_(port),
          is_connected_(false),
          state_(client::connection_state::initial),
          writer_(std::make_shared<detail::async_writer>(
              &io_, CALLME_ASIO::ip::tcp::socket(io_))) {
        pac_.reserve_buffer(default_buffer_size);
    }

    void do_connect(tcp::resolver::iterator endpoint_iterator) {
        LOG_INFO("Initiating connection.");
        CALLME_ASIO::async_connect(
            writer_->socket_, endpoint_iterator,
            [this](std::error_code ec, tcp::resolver::iterator) {
                if (!ec) {
                    std::unique_lock<std::mutex> lock(mut_connection_finished_);
                    LOG_INFO("Client connected to {}:{}", addr_, port_);
                    is_connected_ = true;
                    state_ = client::connection_state::connected;
                    conn_finished_.notify_all();
                    do_read();
                } else {
                    LOG_ERROR("Error during connection: {}", ec);
                }
            });
    }

    void do_read() {
        LOG_TRACE("do_read");
        writer_->socket_.async_read_some(
            CALLME_ASIO::buffer(pac_.buffer(), default_buffer_size),
            [this](std::error_code ec, std::size_t length) {
                if (!ec) {
                    pac_.buffer_consumed(length);
                    if (length >= pac_.buffer_capacity()) {
                        auto new_size = static_cast<std::size_t>(
                            pac_.buffer_capacity() * buffer_grow_factor);
                        LOG_INFO("Resizing buffer to {}", new_size);
                        pac_.reserve_buffer(new_size);
                    }

                    LOG_TRACE("Read chunk of size {}", length);
                    msgpack::unpacked result;
                    while (pac_.next(&result)) {
                        auto r = response(std::move(result));
                        auto id = r.get_id();
                        auto &c = ongoing_calls_[id];
                        try {
                            if (auto e = r.get_error()) {
								std::cout << e->get() << std::endl;
                                throw rpc_error("callme::rpc_error during call",
                                                std::get<0>(c), e);
                            }
                            std::get<1>(c).set_value(std::move(*r.get_result()));
                        } catch (...) {
                            std::get<1>(c).set_exception(std::current_exception());
                        }
                        strand_.post(
                            [this, id]() { ongoing_calls_.erase(id); });
                    }
                    do_read();
                } else if (ec == CALLME_ASIO::error::eof) {
                    LOG_WARN("The server closed the connection.");
                    state_ = client::connection_state::disconnected;
                } else if (ec == CALLME_ASIO::error::connection_reset) {
                    state_ = client::connection_state::disconnected;
                    LOG_WARN("The connection was reset.");
                } else {
                    LOG_ERROR("Unhandled error code: {}", ec);
                }
            });
    }

    client::connection_state get_connection_state() const { return state_; }

    //! \brief Waits for the write queue and writes any buffers to the network
    //! connection. Should be executed throught strand_.
    void write(msgpack::sbuffer item) { writer_->write(std::move(item)); }

    using call_t = std::pair<std::string, std::promise<msgpack::object_handle>>;

    client *parent_;
    CALLME_ASIO::io_service io_;
    CALLME_ASIO::strand strand_;
    std::atomic<int> call_idx_; /// The index of the last call made
    std::unordered_map<uint32_t, call_t> ongoing_calls_;
    std::string addr_;
    uint16_t port_;
    msgpack::unpacker pac_;
    std::atomic_bool is_connected_;
    std::condition_variable conn_finished_;
    std::mutex mut_connection_finished_;
    std::thread io_thread_;
    std::atomic<client::connection_state> state_;
    std::shared_ptr<detail::async_writer> writer_;
    CALLME_CREATE_LOG_CHANNEL(client)
};

client::client(std::string const &addr, uint16_t port)
    : pimpl(this, addr, port) {
    tcp::resolver resolver(pimpl->io_);
    auto endpoint_it =
        resolver.resolve({pimpl->addr_, std::to_string(pimpl->port_)});
    pimpl->do_connect(endpoint_it);
    std::thread io_thread([this]() {
        CALLME_CREATE_LOG_CHANNEL(client)
        name_thread("client");
        pimpl->io_.run();
    });
    pimpl->io_thread_ = std::move(io_thread);
}

void client::wait_conn() {
    std::unique_lock<std::mutex> lock(pimpl->mut_connection_finished_);
    if (!pimpl->is_connected_) {
        pimpl->conn_finished_.wait(lock);
    }
}

int client::get_next_call_idx() {
    ++(pimpl->call_idx_);
    return pimpl->call_idx_;
}

void client::post(std::shared_ptr<msgpack::sbuffer> buffer, int idx,
                  std::string const &func_name,
                  std::shared_ptr<rsp_promise> p) {
    pimpl->strand_.post([=]() {
        pimpl->ongoing_calls_.insert(
            std::make_pair(idx, std::make_pair(func_name, std::move(*p))));
        pimpl->write(std::move(*buffer));
    });
}

void client::post(msgpack::sbuffer *buffer) {
    pimpl->strand_.post([=]() {
        pimpl->write(std::move(*buffer));
        delete buffer;
    });
}

client::connection_state client::get_connection_state() const {
    return pimpl->get_connection_state();
}

void client::wait_all_responses() {
    for (auto &c : pimpl->ongoing_calls_) {
        c.second.second.get_future().wait();
    }
}

client::~client() {
    pimpl->io_.stop();
    pimpl->io_thread_.join();
}
}
