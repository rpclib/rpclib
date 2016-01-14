#pragma once

#ifndef TESTUTILS_H_LHCAMVUX
#define TESTUTILS_H_LHCAMVUX

#include <tuple>
#include <thread>
#include "msgpack.hpp"
#include "callme/detail/uv_loop.h"
#include "callme/detail/uv_adaptor.h"

using namespace callme::detail;

namespace callme {
namespace testutils {

//! \brief Creates a unpacked messagepack containing its arguments.
template <typename... Types>
inline msgpack::unpacked make_unpacked(Types... items) {
    auto obj = std::make_tuple(items...);
    msgpack::sbuffer buf;
    msgpack::pack(buf, obj);
    msgpack::unpacked msg;
    msgpack::unpack(&msg, buf.data(), buf.size());
    return msg;
}

//! \brief Creates a packed messagepack containing its arguments and returns the
//! buffer containing it.
template <typename... Types>
inline msgpack::sbuffer make_packed(Types... items) {
    auto obj = std::make_tuple(items...);
    msgpack::sbuffer buf;
    msgpack::pack(buf, obj);
    return buf;
}

//! \brief Class for testing tcp client and/or server functionality.
class tcp_tester : public testing::Test,
                   public callme::detail::uv_adaptor<tcp_tester> {
public:
    tcp_tester()
        : callme::detail::uv_adaptor<tcp_tester>(),
          server_(uv_loop::instance().make_handle<uv_tcp_t>(this)),
          client_(uv_loop::instance().make_handle<uv_tcp_t>(this)),
          incoming_(uv_loop::instance().make_handle<uv_tcp_t>(this)),
          connect_(uv_loop::instance().make_handle<uv_connect_t>(this)),
          read_finished_(false), is_running_(false) {}

    ~tcp_tester() { LOG_INFO("tcp_tester dtor"); }

    void start_client() {
        struct sockaddr_in addr;
        uv_ip4_addr("127.0.0.1", test_port, &addr);
        uv_tcp_init(uv_loop::instance().get_loop(), client_);

        uv_tcp_connect(connect_, client_,
                       reinterpret_cast<const sockaddr *>(&addr), nullptr);
    }

    void start_server() {
        struct sockaddr_in addr;

        uv_ip4_addr("0.0.0.0", test_port, &addr);

        uv_tcp_init(uv_loop::instance().get_loop(), server_);
        uv_tcp_bind(server_, (struct sockaddr *)&addr, 0);
        uv_listen((uv_stream_t *)server_, 128,
                  &tcp_tester::fw_on_new_connection);
    }

    void run_test_loop() { uv_loop::instance().start(); }

protected:
    void on_new_connection(uv_stream_t *tcp, int status) {
        uv_tcp_init(uv_loop::instance().get_loop(), incoming_);
        uv_accept(tcp, (uv_stream_t *)incoming_);
        uv_read_start(reinterpret_cast<uv_stream_t *>(incoming_),
                      &tcp_tester::fw_alloc_buffer, &tcp_tester::fw_on_read);
    }

    void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
        read_buf_.resize(nread);
        memcpy(read_buf_.data(), buf->base, nread);

        read_finished_ = true; // technically, it would be possible that there
                               // were mulitple on_read callbacks, but the data
                               // is short so there is no point in complicating
                               // this.
    }

    void alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t *buf) {
        recv_buf_.clear();
        recv_buf_.reserve(size);
        *buf = uv_buf_init(recv_buf_.data(), size);
    }

    static const uint16_t test_port = 8080;
    uv_tcp_t *server_;
    uv_tcp_t *client_;
    uv_tcp_t *incoming_;
    uv_connect_t *connect_;
    std::atomic<bool> read_finished_, is_running_;
    std::vector<char> recv_buf_, read_buf_;
    friend class callme::detail::uv_adaptor<tcp_tester>;
};
}
}


#endif /* end of include guard: TESTUTILS_H_LHCAMVUX */
