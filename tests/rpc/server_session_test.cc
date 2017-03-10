#include "gtest/gtest.h"

#include "rpc/client.h"
#include "rpc/server.h"
#include "testutils.h"

using namespace rpc::testutils;

class server_session_test : public testing::Test {
public:
    server_session_test() :
            s("127.0.0.1", test_port),
            c("127.0.0.1", test_port) {
        s.bind("consume_big_param", [](std::string const& str){ (void)str; });
        s.async_run();
    }

protected:
    static const int test_port = 8080;
    rpc::server s;
    rpc::client c;
};

TEST_F(server_session_test, consume_big_param) {
    std::size_t blob_size = 2 << 10 << 10;
    for (int i = 0; i < 4; ++i) {
        c.call("consume_big_param", get_blob(blob_size));
        blob_size *= 2;
    }
    // no crash is enough
}
