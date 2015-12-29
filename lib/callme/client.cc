#include "callme/client.h"
#include "uv.h"
#include "callme/string_ref.h"
#include "callme/detail/log.h"

namespace callme
{

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

void client::on_connect(uv_connect_t *request, int status) {
    LOG_INFO("Client connected with status %d", status);
    delete request;
}

void client::on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
}

void client::alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t *buffer) {
    LOG_TRACE("Allocating %v bytes", size);
    pac_.reserve_buffer(size);
    *buffer = uv_buf_init(pac_.buffer(), size);
}

void client::on_write(uv_write_t *req, int status) {
}

void client::run() {
    uv_run(loop_, UV_RUN_DEFAULT);
}

}
