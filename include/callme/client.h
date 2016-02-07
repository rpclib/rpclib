#pragma once

#include "asio.hpp"
#include "msgpack.hpp"

#include <atomic>
#include <condition_variable>
#include <deque>
#include <future>
#include <mutex>
#include <unordered_map>

#include "callme/detail/log.h"
#include "callme/detail/thread_group.h"

namespace callme {

//! \brief Implements a client that connects to a msgpack-rpc server and is
//! able to call functions synchronously or asynchronously.
class client {
public:
    client(std::string const &addr, uint16_t port);

    client(client const &) = delete;

    ~client();

    //! \brief Calls a function with the given name and arguments (if any).
    //! \param func_name The name of the function to call.
    //! \param args The arguments to pass to the function.
    //! \returns A msgpack::object containing the result of the function (if
    //! any).
    //! To obtain a typed value, use the msgpack API.
    //! \tparam Args THe types of the arguments.
    template <typename... Args>
    msgpack::object call(std::string const &func_name, Args... args);

    //! \brief Calls a function asynchronously with the given name and
    //! arguments.
    //! \param func_name The name of the function to call.
    //! \param args The arguments to pass to the function.
    //! \returns A callme::maybe object, possibly holding a future result
    //! (which is a msgpack::object). \see callme::maybe.
    //! \tparam Args THe types of the arguments.
    template <typename... Args>
    std::future<msgpack::object> call_async(std::string const &func_name,
                                            Args... args);

private:
    enum class call_type { sync = 0, async = 2 };

    void do_connect(asio::ip::tcp::resolver::iterator endpoint_iterator);

    void do_read();

    //! \brief Puts the current thread into waiting state until the client
    //! connects.
    void wait_conn();

    //! \brief Waits for the write queue and writes any buffers to the network
    //! connection. Should be executed throught strand_.
    void write(std::unique_ptr<msgpack::sbuffer> item);

    //! \brief Performs writing the items of the write queue in order.
    void do_write();

private:
    asio::io_service io_;
    asio::strand strand_;
    asio::ip::tcp::socket socket_;
    std::string addr_;
    uint16_t port_;
    msgpack::unpacker pac_;
    msgpack::sbuffer buf_;

    static const uint32_t default_buffer_size = 4096;

    std::atomic<int> call_idx_; //< The index of the last call made
    std::atomic_bool exiting_;
    std::atomic_flag write_running_ = ATOMIC_FLAG_INIT;
    std::unordered_map<int, std::promise<msgpack::object>> ongoing_calls_;

    std::atomic_bool is_connected_;
    std::condition_variable conn_finished_;
    std::mutex mut_connection_finished_;
    std::thread io_thread_, write_thread_;

    std::deque<std::unique_ptr<msgpack::sbuffer>> write_queue_;

    CALLME_CREATE_LOG_CHANNEL(client)
};
}

#include "callme/client.inl"
