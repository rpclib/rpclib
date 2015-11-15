#include <functional>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "callme/server.h"
#include "callme/util/make_array.h"

// Unit tests for binding functions to names.
// A couple of these tests exist to check if the code compiles correctly,
// obviously that is not in the scope of normal unit testing.

void dummy_void_zeroarg(){};
void dummy_void_singlearg(int x){};
void dummy_void_multiarg(int x, int y){};

class binding_test : public testing::Test {
public:
    binding_test() : server("localhost", 8080) {}

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
        0x69, 0x64, 0x5f, 0x6e, 0x6f, 0x70, 0x61, 0x72, 0x61, 0x6d, 0x90};
    server.bind("dummy_void_zeroarg", &dummy_void_zeroarg);
    raw_call(raw_msg);
}

TEST_F(binding_test, freefunc_void_singlearg) {
    const unsigned char raw_msg[] = {
        0x94, 0x1,  0x0,  0xb2, 0x64, 0x75, 0x6d, 0x6d, 0x79, 0x5f, 0x76, 0x6f,
        0x69, 0x64, 0x5f, 0x6e, 0x6f, 0x70, 0x61, 0x72, 0x61, 0x6d, 0x90};
    server.bind("dummy_void_singlearg", &dummy_void_singlearg);
    raw_call(raw_msg);
}

//TEST_F(binding_test, freefunc_void_multiarg) {
    //const unsigned char raw_msg[] = {
        //0x94, 0x1,  0x0,  0xb2, 0x64, 0x75, 0x6d, 0x6d, 0x79, 0x5f, 0x76, 0x6f,
        //0x69, 0x64, 0x5f, 0x6e, 0x6f, 0x70, 0x61, 0x72, 0x61, 0x6d, 0x90};
    //server.bind("dummy_void_multiarg", &dummy_void_multiarg);
    //raw_call(raw_msg);
//}

// struct IDummy {
// virtual void dummy_void(int x) = 0;
// virtual int dummy_int(int x) = 0;
// virtual void dummy_void_noparam() = 0;
//};

// struct MockDummy : IDummy {
// MOCK_METHOD1(dummy_void, void(int));
// MOCK_METHOD1(dummy_int, int(int));
// MOCK_METHOD0(dummy_void_noparam, void());
//};

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
