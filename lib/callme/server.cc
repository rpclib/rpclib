#include "callme/server.h"
#include <stdexcept>
#include "loguru.hpp"

inline bool is_success(int result) { return result == 0; }
inline bool is_error(int result) { return result < 0; }

namespace callme {

server::server(boost::string_ref address, uint16_t port)
    : loop_(uv_default_loop()) {
    const unsigned no_flag = 0;
    sockaddr_in addr;
    uv_ip4_addr(&address.front(), port, &addr);
    uv_tcp_init(loop_, &tcp_);
    uv_tcp_bind(&tcp_, (sockaddr * const) & addr, no_flag);
    tcp_.data = this;
}

void server::fw_on_new_connection(uv_stream_t *stream, int status) {
    server *obj = reinterpret_cast<server *>(stream->data);
    obj->on_new_connection(stream, status);
}

void server::fw_on_read(uv_stream_t *stream, ssize_t nread,
                        const uv_buf_t *buf) {
    server *obj = reinterpret_cast<server *>(stream->data);
    obj->on_read(stream, nread, buf);
}

void server::fw_alloc_buffer(uv_handle_t *handle, size_t size,
                             uv_buf_t *buffer) {
    server *obj = reinterpret_cast<server *>(handle->data);
    obj->alloc_buffer(handle, size, buffer);
}

void server::on_new_connection(uv_stream_t *stream, int status) {
    LOG_SCOPE_FUNCTION(INFO);
    DLOG_F(INFO, "New connection. Status: %d", status);
    if (is_error(status)) {
        DLOG_F(ERROR, "Error while listening. libuv says: %s",
               uv_strerror(status));
        throw std::runtime_error("Error while listening.");
        // TODO: more info in exception [sztomi, 2015-11-21]
    }

    uv_tcp_t *client = static_cast<uv_tcp_t *>(malloc(sizeof(uv_tcp_t)));
    uv_tcp_init(loop_, client);

    if (uv_accept(stream, reinterpret_cast<uv_stream_t *>(client)) == 0) {
        int result = uv_read_start(reinterpret_cast<uv_stream_t *>(client),
                                   &server::fw_alloc_buffer, &server::fw_on_read);
        if (!is_success(result)) {
            DLOG_F(ERROR, "Error while accepting. libuv says: %s",
                   uv_strerror(result));
            throw std::runtime_error("Error while accepting client.");
            // TODO: more info in exception [sztomi, 2015-11-21]
        }
    } else {
        uv_close(reinterpret_cast<uv_handle_t *>(client), nullptr);
    }
}

void server::on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
    LOG_SCOPE_FUNCTION(INFO);
    DLOG_F(8, "Reading from tcp.");
    DLOG_F(8, "nread = %ld", nread);

    if (is_error(nread)) {
        if (nread != UV_EOF) {
            DLOG_F(ERROR, "Reading error. libuv says: %s", uv_strerror(nread));
            throw std::runtime_error("Error while reading");
        }
        uv_close(reinterpret_cast<uv_handle_t *>(stream), nullptr);
    }

    pac_.buffer_consumed(nread); // alloc_buffer has set buf->base to the
                                 // internal buffer of pac_ so it is already
                                 // consumed at this point. No need to copy.
    msgpack::unpacked result;
    while (pac_.next(&result)) {
        auto msg = result.get();
        disp_.dispatch(msg);
    }
}

void server::alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t *buffer) {
    pac_.reserve_buffer(size);
    buffer->base = pac_.buffer();
    buffer->len = size;
}

void server::run() {
    const int default_backlog = 128;
    uv_listen(reinterpret_cast<uv_stream_t *>(&tcp_), default_backlog,
              &server::fw_on_new_connection);
}

} /* callme */
