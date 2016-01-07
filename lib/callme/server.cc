#include "callme/server.h"
#include <stdexcept>

#include "callme/detail/log.h"
#include "callme/detail/uv_loop.h"
#include "format.h"

static inline bool is_success(int result) { return result == 0; }
static inline bool is_error(int result) { return result < 0; }

using namespace callme::detail;

namespace callme {

server::server(std::string const &address, uint16_t port)
    : detail::uv_adaptor<server>(), pac_(), suppress_exceptions_(false) {
    LOG_INFO("Created server on address %v:%v", address, port);
    const unsigned no_flag = 0;
    sockaddr_in *addr = new sockaddr_in;
    uv_ip4_addr(&address.front(), port, addr);
    tcp_ = uv_loop::instance().make_handle<uv_tcp_t>();
    uv_tcp_init(uv_loop::instance().get_loop(), tcp_);
    uv_tcp_bind(tcp_, (sockaddr * const)addr, no_flag);
}

void server::suppress_exceptions(bool suppress) {
    suppress_exceptions_ = suppress;
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

    uv_tcp_t *client = uv_loop::instance().make_handle<uv_tcp_t>(this);
    uv_tcp_init(uv_loop::instance().get_loop(), client);
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
        auto msg = result.get();
        LOG_DEBUG("msgpack read from tcp.");
        try {
            auto resp = disp_.dispatch(msg);
            resp.write(stream);
        } catch (std::exception &e) {
            dispatcher::msg_type the_call;
            msg.convert(&the_call);
            // TODO: This is a bit redundant because the dispatcher does this,
            // too. There is a chance that we have to fuse the
            // dispatcher and server classes [t.szelei 2015-12-09]
            auto &&id = std::get<1>(the_call);
            auto &&name = std::get<2>(the_call);
            auto &&args = std::get<3>(the_call);

            response error_resp(
                id, fmt::format("callme: function '{0}' (taking {1} arg(s)) "
                                "threw an exception. The exception "
                                "contained this information: {2}.",
                                name, args.via.array.size, e.what()),
                std::make_unique<msgpack::object>());
            error_resp.write(stream);

            if (!suppress_exceptions_) {
                throw;
            }
        }
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
    uv_listen(reinterpret_cast<uv_stream_t *>(tcp_), default_backlog,
              &server::fw_on_new_connection);
    uv_loop::instance().start();
}

} /* callme */
