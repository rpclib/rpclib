#include "callme/client.h"
#include "callme/response.h"
#include "uv.h"
#include "callme/string_ref.h"
#include "callme/detail/log.h"
#include <thread>

#include "format.h"

static inline bool is_success(int result) { return result == 0; }
static inline bool is_error(int result) { return result < 0; }

namespace callme {

client::client(string_ref addr, uint16_t port)
    : addr_(addr), port_(port), loop_(uv_default_loop()) {
    uv_tcp_init(loop_, &tcp_);
    uv_tcp_keepalive(&tcp_, 1, 60);

    uv_connect_t *connect = new uv_connect_t;

    struct sockaddr_in dest;
    uv_ip4_addr(&addr_.front(), port_, &dest);

    tcp_.data = this;
    uv_tcp_connect(connect, &tcp_, reinterpret_cast<const sockaddr *>(&dest),
                   &client::fw_on_connect);
}

client::~client() {
    LOG_INFO("Closing connection");
    uv_close(reinterpret_cast<uv_handle_t *>(&tcp_), &client::fw_on_close);
}

void client::on_connect(uv_connect_t *request, int status) {
    LOG_INFO("Client connected with status %v", status);

    uv_read_start(request->handle, &client::fw_alloc_buffer,
                  &client::fw_on_read);
}

void client::on_close(uv_handle_t *handle) {
    std::unique_lock<std::mutex> lk(close_finish_mut_);
    LOG_INFO("Connection closed.");
    close_finish_.notify_one();
}

void client::on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
    LOG_INFO("Reading from tcp. nread = %v", nread);

    if (nread == UV_EOF) {
        LOG_INFO("End of file.");
        return;
    }

    if (is_error(nread)) {
        if (nread != UV_EOF) {
            auto err = fmt::format("Error while reading. libuv says: {}",
                                   uv_strerror(nread));
            LOG_ERROR(err);
            throw std::runtime_error(err);
        } else {
            uv_close(reinterpret_cast<uv_handle_t *>(stream), nullptr);
            return;
        }
    }

    pac_.buffer_consumed(nread); // alloc_buffer has set buf->base to the
                                 // internal buffer of pac_ so it is already
                                 // consumed at this point. No need to copy...
    msgpack::unpacked result;
    while (pac_.next(&result)) {
        auto r = response(result.get());
        auto &promise = ongoing_calls_[r.get_id()];
        try {
            if (r.get_error().size() > 0) {
                throw std::runtime_error(
                    fmt::format("callme: error during RPC call: %v",
                                r.get_error().to_string()));
            }
            promise.set_value(r.get_result());
        } catch (...) {
            promise.set_exception(std::current_exception());
        }
    }

    // and no need to deallocate buf->base here
}

void client::alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t *buffer) {
    LOG_TRACE("Allocating %v bytes", size);
    pac_.reserve_buffer(size);
    *buffer = uv_buf_init(pac_.buffer(), size);
}

void client::on_write(uv_write_t *req, int status) {
    LOG_DEBUG("Writing to tcp. Status: %v", status);
}

void client::run() { uv_run(loop_, UV_RUN_DEFAULT); }
}
