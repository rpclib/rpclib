#include "callme/server.h"
#include <stdexcept>

#include "callme/detail/log.h"
#include "format.h"

static inline bool is_success(int result) { return result == 0; }
static inline bool is_error(int result) { return result < 0; }

namespace callme {

server::server(boost::string_ref address, uint16_t port)
    : loop_(uv_default_loop()), pac_(),
      exc_strat_(exception_strategy::response_rethrow) {
    LOG_INFO("Created server on address %v:%v", address.to_string(), port);
    const unsigned no_flag = 0;
    sockaddr_in *addr = new sockaddr_in;
    uv_ip4_addr(&address.front(), port, addr);
    uv_tcp_init(loop_, &tcp_);
    uv_tcp_bind(&tcp_, (sockaddr * const)addr, no_flag);
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
    LOG_INFO("New connection. Status: %v", status);
    if (is_error(status)) {
        auto err =
            fmt::format("Error while opening new connection. libuv says: {}",
                        uv_strerror(status));
        LOG_ERROR(err);
        throw std::runtime_error(err.c_str());
    }

    uv_tcp_t *client = static_cast<uv_tcp_t *>(malloc(sizeof(uv_tcp_t)));
    uv_tcp_init(loop_, client);
    client->data = this;

    if (uv_accept(stream, reinterpret_cast<uv_stream_t *>(client)) == 0) {
        LOG_INFO("Accepted connection.");
        int result =
            uv_read_start(reinterpret_cast<uv_stream_t *>(client),
                          &server::fw_alloc_buffer, &server::fw_on_read);
        if (!is_success(result)) {
            auto err = fmt::format("Error while accepting. libuv says: {}",
                                   uv_strerror(result));
            LOG_ERROR(err);
            throw std::runtime_error(err.c_str());
        }
    } else {
        uv_close(reinterpret_cast<uv_handle_t *>(client), nullptr);
    }
}

void server::on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
    LOG_INFO("Reading from tcp. nread = %v", nread);

    if (is_error(nread)) {
        if (nread != UV_EOF) {
            auto err = fmt::format("Error while reading. libuv says: {}",
                                   uv_strerror(nread));
            LOG_ERROR(err);
            throw std::runtime_error(err);
        }
        uv_close(reinterpret_cast<uv_handle_t *>(stream), nullptr);
    }

    pac_.buffer_consumed(nread); // alloc_buffer has set buf->base to the
                                 // internal buffer of pac_ so it is already
                                 // consumed at this point. No need to copy...
    msgpack::unpacked result;
    while (pac_.next(&result)) {
        auto msg = result.get();
        LOG_DEBUG("Dispatching call");
        disp_.dispatch(msg).write(stream);
    }

    // and no need to deallocate buf->base here
}

void server::alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t *buffer) {
    LOG_TRACE("Allocating %v bytes", size);
    pac_.reserve_buffer(size);
    *buffer = uv_buf_init(pac_.buffer(), size);
}

void server::run() {
    const int default_backlog = 128;
    uv_listen(reinterpret_cast<uv_stream_t *>(&tcp_), default_backlog,
              &server::fw_on_new_connection);
    uv_run(loop_, UV_RUN_DEFAULT);
}

server::exception_strategy server::get_exception_strategy() const {
    return exc_strat_;
}

void server::set_exception_strategy(server::exception_strategy s) {
    exc_strat_ = s;
}

} /* callme */
