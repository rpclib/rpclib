#include "gtest/gtest.h"

#include <memory>
#include <atomic>
#include <thread>

#include "callme/response.h"
#include "callme/detail/uv_adaptor.h"
#include "uv.h"
#include "testutils.h"

using namespace callme::testutils;

class tcp_tester : public testing::Test,
                   public callme::detail::uv_adaptor<tcp_tester> {
public:
    tcp_tester() : write_finished(false) {}

    ~tcp_tester() {
        uv_close(reinterpret_cast<uv_handle_t *>(&server), 0);
        uv_close(reinterpret_cast<uv_handle_t *>(&client), 0);
        uv_close(reinterpret_cast<uv_handle_t *>(&incoming), 0);
    }

    void start_client() {
        struct sockaddr_in addr;
        uv_ip4_addr("127.0.0.1", test_port, &addr);
        client.data = this;
        uv_tcp_init(uv_default_loop(), &client);

        uv_connect_t *connect = new uv_connect_t;
        uv_tcp_connect(connect, &client,
                       reinterpret_cast<const sockaddr *>(&addr), nullptr);
    }

    void start_server() {
        struct sockaddr_in addr;

        uv_ip4_addr("0.0.0.0", test_port, &addr);

        uv_tcp_init(uv_default_loop(), &server);
        server.data = this;
        uv_tcp_bind(&server, (struct sockaddr *)&addr, 0);
        uv_listen((uv_stream_t *)&server, 128,
                  &tcp_tester::fw_on_new_connection);
    }

    void run() { uv_run(uv_default_loop(), UV_RUN_DEFAULT); }

protected:
    void on_new_connection(uv_stream_t *tcp, int status) {
        uv_tcp_init(tcp->loop, &incoming);
        uv_accept(tcp, (uv_stream_t *)&incoming);
        incoming.data = this;
        uv_read_start(reinterpret_cast<uv_stream_t *>(&incoming),
                      &tcp_tester::fw_alloc_buffer, &tcp_tester::fw_on_read);
    }

    void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
        read_buffer.resize(nread);
        memcpy(read_buffer.data(), buf->base, nread);

        write_finished = true; // technically, it would be possible that there
                               // were mulitple on_read callbacks, but the data
                               // is short so there is no point in complicating
                               // this.
    }

    void alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t *buf) {
        buffer.clear();
        buffer.reserve(size);
        *buf = uv_buf_init(buffer.data(), size);
    }

    static const uint16_t test_port = 8080;
    uv_tcp_t server;
    uv_tcp_t client;
    uv_tcp_t incoming;
    uv_write_t write_req;
    std::atomic<bool> write_finished;
    std::vector<char> buffer, read_buffer;
    friend class callme::detail::uv_adaptor<tcp_tester>;
};

TEST(response, object_ctor) {
    auto o = make_obj(3, 42, "foo", "bar");
    callme::response r(o.get());
    EXPECT_EQ(r.get_id(), 42);
    EXPECT_TRUE(r.get_error() == "foo");

    std::string result;
    r.get_result().convert(&result);
    EXPECT_TRUE(result == "bar");
}

class response_write_test : public tcp_tester {};

TEST_F(response_write_test, writing) {
    start_server();
    start_client();
    std::thread t([this]() { run(); });
    auto obj = make_obj(1, 42, "foo", "bar");
    callme::response r(obj.get());
    r.write(reinterpret_cast<uv_stream_t *>(&client));

    callme::response::response_type same_obj(1, 42, msgpack::object("foo"),
                                             msgpack::object("bar"));
    msgpack::sbuffer buf;
    msgpack::pack(buf, same_obj);

    while (!write_finished) {
        // nop
    }

    uv_stop(uv_default_loop());
    t.join();

    EXPECT_EQ(buf.size(), read_buffer.size());
    EXPECT_EQ(0, memcmp(buf.data(), read_buffer.data(), buf.size()));
}
