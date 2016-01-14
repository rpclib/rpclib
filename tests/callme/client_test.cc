#include "gtest/gtest.h"

#include "callme/client.h"
#include "testutils.h"

using namespace callme::testutils;

class client_test : public tcp_tester {};

TEST_F(client_test, instantiation) {
    start_server();
    callme::client client("127.0.0.1", test_port);
}
