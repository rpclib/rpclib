#include <functional>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "callme/server.h"
#include "callme/util/make_array.h"

// Unit tests for binding functions to names.
// A couple of these tests exist to check if the code compiles correctly,
// obviously that is not in the scope of normal unit testing.

bool g_dummy_void_zeroarg_called;
bool g_dummy_void_singlearg_called;
bool g_dummy_void_multiarg_called;
void dummy_void_zeroarg() { g_dummy_void_zeroarg_called = true; };
void dummy_void_singlearg(int x) { g_dummy_void_singlearg_called = true; };
void dummy_void_multiarg(int x, int y) { g_dummy_void_multiarg_called = true; };

class binding_test : public testing::Test {
public:
    binding_test() : server("localhost", 8080) {
        g_dummy_void_zeroarg_called = false;
        g_dummy_void_singlearg_called = false;
        g_dummy_void_multiarg_called = false;
    }

    template <typename A> void raw_call(A &&msg_array) {
        msgpack::sbuffer msg;
        msg.write(reinterpret_cast<const char *>(msg_array), sizeof(msg_array));

        server.process_call(msg);
    }

protected:
    callme::server server;
};

TEST_F(binding_test, freefunc_void_zeroarg) {
    const unsigned char raw_msg[] = {
        0x94, 0x1,  0x0,  0xb2, 0x64, 0x75, 0x6d, 0x6d, 0x79, 0x5f, 0x76, 0x6f,
        0x69, 0x64, 0x5f, 0x7a, 0x65, 0x72, 0x6f, 0x61, 0x72, 0x67, 0x90};
    server.bind("dummy_void_zeroarg", &dummy_void_zeroarg);
    raw_call(raw_msg);
    EXPECT_TRUE(g_dummy_void_zeroarg_called);
}

TEST_F(binding_test, freefunc_void_singlearg) {
    const unsigned char raw_msg[] = {0x94, 0x1,  0x0,  0xb4, 0x64, 0x75, 0x6d,
                                     0x6d, 0x79, 0x5f, 0x76, 0x6f, 0x69, 0x64,
                                     0x5f, 0x73, 0x69, 0x6e, 0x67, 0x6c, 0x65,
                                     0x61, 0x72, 0x67, 0x91, 0x2a};
    server.bind("dummy_void_singlearg", &dummy_void_singlearg);
    raw_call(raw_msg);
    EXPECT_TRUE(g_dummy_void_singlearg_called);
}

TEST_F(binding_test, freefunc_void_multiarg) {
    const unsigned char raw_msg[] = {0x94, 0x1,  0x0,  0xb3, 0x64, 0x75, 0x6d,
                                     0x6d, 0x79, 0x5f, 0x76, 0x6f, 0x69, 0x64,
                                     0x5f, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x61,
                                     0x72, 0x67, 0x92, 0xcd, 0x1,  0x6b, 0xc};
    server.bind("dummy_void_multiarg", &dummy_void_multiarg);
    raw_call(raw_msg);
    EXPECT_TRUE(g_dummy_void_multiarg_called);
}

struct IDummy {
    virtual void dummy_void_zeroarg() = 0;
    virtual void dummy_void_singlearg(int x) = 0;
    virtual void dummy_void_multiarg(int x, int y) = 0;
};

struct MockDummy : IDummy {
    MOCK_METHOD0(dummy_void_zeroarg, void());
    MOCK_METHOD1(dummy_void_singlearg, void(int));
    MOCK_METHOD2(dummy_void_multiarg, void(int, int));
};

TEST_F(binding_test, memfunc_void_zeroarg) {
    MockDummy md;
    EXPECT_CALL(md, dummy_void_zeroarg());

    const unsigned char raw_msg[] = {
        0x94, 0x1,  0x0,  0xb2, 0x64, 0x75, 0x6d, 0x6d, 0x79, 0x5f, 0x76, 0x6f,
        0x69, 0x64, 0x5f, 0x7a, 0x65, 0x72, 0x6f, 0x61, 0x72, 0x67, 0x90};
    server.bind("dummy_void_zeroarg", [&md]() { md.dummy_void_zeroarg(); });
    raw_call(raw_msg);
}

TEST_F(binding_test, memfunc_void_singlearg) {
    MockDummy md;
    EXPECT_CALL(md, dummy_void_singlearg(42));

    const unsigned char raw_msg[] = {0x94, 0x1,  0x0,  0xb4, 0x64, 0x75, 0x6d,
                                     0x6d, 0x79, 0x5f, 0x76, 0x6f, 0x69, 0x64,
                                     0x5f, 0x73, 0x69, 0x6e, 0x67, 0x6c, 0x65,
                                     0x61, 0x72, 0x67, 0x91, 0x2a};
    server.bind("dummy_void_singlearg",
                [&md](int x) { md.dummy_void_singlearg(x); });
    raw_call(raw_msg);
}

TEST_F(binding_test, memfunc_void_multiarg) {
    MockDummy md;
    EXPECT_CALL(md, dummy_void_multiarg(362, 12));

    const unsigned char raw_msg[] = {0x94, 0x1,  0x0,  0xb3, 0x64, 0x75, 0x6d,
                                     0x6d, 0x79, 0x5f, 0x76, 0x6f, 0x69, 0x64,
                                     0x5f, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x61,
                                     0x72, 0x67, 0x92, 0xcd, 0x1,  0x6b, 0xc};
    raw_call(raw_msg);
}

// TEST(server, call_void) {
// callme::server server("localhost", 8080);

// MockDummy md;
// server.bind("dummy_void", [&md](int x) { md.dummy_void(x); });
// EXPECT_CALL(md, dummy_void(42));

//// this buffer contains a call to dummy_void with 42 as the argument
// const unsigned char raw_msg[] = {0x94, 0x1,  0x0,  0xaa, 0x64,
// 0x75, 0x6d, 0x6d, 0x79, 0x5f,
// 0x76, 0x6f, 0x69, 0x64, 0x2a};
// msgpack::sbuffer msg;
// msg.write(reinterpret_cast<const char *>(raw_msg), sizeof(raw_msg));

// server.process_call(msg);
//}

// TEST(server, call_non_void) {
// callme::server server("localhost", 8080);

// MockDummy md;
// server.bind("dummy_int", [&md](int x) { return md.dummy_int(x); });
// EXPECT_CALL(md, dummy_int(24));
// const unsigned char raw_msg[] = {0x94, 0x1,  0x0,  0xa9, 0x64, 0x75, 0x6d,
// 0x6d, 0x79, 0x5f, 0x69, 0x6e, 0x74, 0x18};

// msgpack::sbuffer msg;
// msg.write(reinterpret_cast<const char *>(raw_msg), sizeof(raw_msg));

// server.process_call(msg);
//}

// TEST(server, call_void_noparam) {
// callme::server server("localhost", 8080);

// MockDummy md;
// server.bind("dummy_void_noparam", [&md]() { md.dummy_void_noparam(); });
// EXPECT_CALL(md, dummy_void_noparam());
// const unsigned char raw_msg[] = {
// 0x94, 0x1,  0x0,  0xb2, 0x64, 0x75, 0x6d, 0x6d, 0x79, 0x5f, 0x76, 0x6f,
// 0x69, 0x64, 0x5f, 0x6e, 0x6f, 0x70, 0x61, 0x72, 0x61, 0x6d, 0x90};

// msgpack::sbuffer msg;
// msg.write(reinterpret_cast<const char *>(raw_msg), sizeof(raw_msg));

// server.process_call(msg);
//}
