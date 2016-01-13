#pragma once

#include "msgpack.hpp"
#include "uv.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <future>
#include <unordered_map>

#include "callme/maybe.h"
#include "callme/detail/uv_adaptor.h"
#include "callme/detail/log.h"

namespace callme {

//! \brief Implements a client that connects to a msgpack-rpc server and is
//! able to call functions synchronously or asynchronously.
class client : public detail::uv_adaptor<client>
{
public:
    client(std::string const& addr, uint16_t port);

    client(client const&) = delete;

    ~client();

    //! \brief Calls a function with the given name and arguments (if any).
    //! \param func_name The name of the function to call.
    //! \param args The arguments to pass to the function.
    //! \returns A msgpack::object containing the result of the function (if any).
    //! To obtain a typed value, use the msgpack API.
    //! \tparam Args THe types of the arguments.
    template<typename... Args>
    msgpack::object call(std::string const& func_name, Args... args);

    //! \brief Calls a function asynchronously with the given name and arguments.
    //! \param func_name The name of the function to call.
    //! \param args The arguments to pass to the function.
    //! \returns A callme::maybe object, possibly holding a future result
    //! (which is a msgpack::object). \see callme::maybe.
    //! \tparam Args THe types of the arguments.
    template<typename... Args>
    maybe call_async(std::string const& func_name, Args... args);

    void run();

    //! \brief Puts the current thread into waiting state until the client connects.
    void wait_conn();

private:
    //! \brief Handles a new connection
    void on_new_connection(uv_stream_t *stream, int status);

    //! \brief Handles reading from a stream.
    void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

    //! \brief Called when a write request finishes.
    void on_write(uv_write_t *req, int status);
    
    void on_connect(uv_connect_t *request, int status);

    void on_close(uv_handle_t *handle);

    //! \brief Allocates a buffer directly inside the unpacker, avoiding a copy.
    void alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t *buffer);

    enum class call_type { sync = 0, async = 2 };

    friend class detail::uv_adaptor<client>;

private:
    std::string addr_;
    uint16_t port_;
    std::atomic<int> call_idx_; //< The index of the last call made
    std::unordered_map<int, std::promise<msgpack::object>> ongoing_calls_;
    msgpack::unpacker pac_;
    msgpack::sbuffer buf_;
    uv_tcp_t* tcp_;
    uv_connect_t* conn_req_;
    std::atomic<bool> is_running_;

    bool is_connected_;
    std::condition_variable conn_finished_;
    std::mutex mut_connection_finished_;
};

}

#include "callme/client.inl"
