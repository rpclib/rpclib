#include <functional>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "callme/dispatcher.h"
#include "testutils.h"

using namespace callme::testutils;

// Unit tests for binding functions to names.
// A couple of these tests exist to check if the code compiles correctly,
// obviously that is not in the scope of normal unit testing.

// Yes, ugly global variables right away. We have to test for free functions
// somehow though :(
bool g_dummy_void_zeroarg_called;
bool g_dummy_void_singlearg_called;
bool g_dummy_void_multiarg_called;
void dummy_void_zeroarg() { g_dummy_void_zeroarg_called = true; }
void dummy_void_singlearg(int x) { g_dummy_void_singlearg_called = true; }
void dummy_void_multiarg(int x, int y) { g_dummy_void_multiarg_called = true; }

class binding_test : public testing::Test {
public:
    binding_test() : dispatcher() {
        g_dummy_void_zeroarg_called = false;
        g_dummy_void_singlearg_called = false;
        g_dummy_void_multiarg_called = false;
    }

    template <typename A> void raw_call(A &&msg_array) {
        msgpack::sbuffer msg;
        msg.write(reinterpret_cast<const char *>(msg_array), sizeof(msg_array));
        dispatcher.dispatch(msg);
    }

protected:
    callme::dispatcher dispatcher;
};

class dispatch_test : public binding_test {};

// The following raw messages were created with the python msgpack
// library from hand-crafted tuples of msgpack-rpc calls.

TEST_F(binding_test, freefunc_void_zeroarg) {
    const unsigned char raw_msg[] = {
        0x94, 0x1,  0x0,  0xb2, 0x64, 0x75, 0x6d, 0x6d, 0x79, 0x5f, 0x76, 0x6f,
        0x69, 0x64, 0x5f, 0x7a, 0x65, 0x72, 0x6f, 0x61, 0x72, 0x67, 0x90};
    dispatcher.bind("dummy_void_zeroarg", &dummy_void_zeroarg);
    raw_call(raw_msg);
    EXPECT_TRUE(g_dummy_void_zeroarg_called);
}

TEST_F(binding_test, freefunc_void_singlearg) {
    const unsigned char raw_msg[] = {0x94, 0x1,  0x0,  0xb4, 0x64, 0x75, 0x6d,
                                     0x6d, 0x79, 0x5f, 0x76, 0x6f, 0x69, 0x64,
                                     0x5f, 0x73, 0x69, 0x6e, 0x67, 0x6c, 0x65,
                                     0x61, 0x72, 0x67, 0x91, 0x2a};
    dispatcher.bind("dummy_void_singlearg", &dummy_void_singlearg);
    raw_call(raw_msg);
    EXPECT_TRUE(g_dummy_void_singlearg_called);
}

TEST_F(binding_test, freefunc_void_multiarg) {
    const unsigned char raw_msg[] = {0x94, 0x1,  0x0,  0xb3, 0x64, 0x75, 0x6d,
                                     0x6d, 0x79, 0x5f, 0x76, 0x6f, 0x69, 0x64,
                                     0x5f, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x61,
                                     0x72, 0x67, 0x92, 0xcd, 0x1,  0x6b, 0xc};
    dispatcher.bind("dummy_void_multiarg", &dummy_void_multiarg);
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
    dispatcher.bind("dummy_void_zeroarg", [&md]() { md.dummy_void_zeroarg(); });
    raw_call(raw_msg);
}

TEST_F(binding_test, memfunc_void_singlearg) {
    MockDummy md;
    EXPECT_CALL(md, dummy_void_singlearg(42));
    const unsigned char raw_msg[] = {0x94, 0x1,  0x0,  0xb4, 0x64, 0x75, 0x6d,
                                     0x6d, 0x79, 0x5f, 0x76, 0x6f, 0x69, 0x64,
                                     0x5f, 0x73, 0x69, 0x6e, 0x67, 0x6c, 0x65,
                                     0x61, 0x72, 0x67, 0x91, 0x2a};
    dispatcher.bind("dummy_void_singlearg",
                    [&md](int x) { md.dummy_void_singlearg(x); });
    raw_call(raw_msg);
}

TEST_F(binding_test, memfunc_void_multiarg) {
    MockDummy md;
    EXPECT_CALL(md, dummy_void_multiarg(363, 12));
    const unsigned char raw_msg[] = {0x94, 0x1,  0x0,  0xb3, 0x64, 0x75, 0x6d,
                                     0x6d, 0x79, 0x5f, 0x76, 0x6f, 0x69, 0x64,
                                     0x5f, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x61,
                                     0x72, 0x67, 0x92, 0xcd, 0x1,  0x6b, 0xc};
    dispatcher.bind("dummy_void_multiarg",
                    [&md](int x, int y) { md.dummy_void_multiarg(x, y); });
    raw_call(raw_msg);
}

TEST_F(binding_test, stdfunc_void_zeroarg) {
    MockDummy md;
    EXPECT_CALL(md, dummy_void_zeroarg());
    const unsigned char raw_msg[] = {
        0x94, 0x1,  0x0,  0xb2, 0x64, 0x75, 0x6d, 0x6d, 0x79, 0x5f, 0x76, 0x6f,
        0x69, 0x64, 0x5f, 0x7a, 0x65, 0x72, 0x6f, 0x61, 0x72, 0x67, 0x90};
    dispatcher.bind(
        "dummy_void_zeroarg",
        std::function<void()>(std::bind(&IDummy::dummy_void_zeroarg, &md)));
    raw_call(raw_msg);
}

TEST_F(binding_test, stdfunc_void_singlearg) {
    using namespace std::placeholders;
    MockDummy md;
    EXPECT_CALL(md, dummy_void_singlearg(42));
    const unsigned char raw_msg[] = {0x94, 0x1,  0x0,  0xb4, 0x64, 0x75, 0x6d,
                                     0x6d, 0x79, 0x5f, 0x76, 0x6f, 0x69, 0x64,
                                     0x5f, 0x73, 0x69, 0x6e, 0x67, 0x6c, 0x65,
                                     0x61, 0x72, 0x67, 0x91, 0x2a};
    dispatcher.bind("dummy_void_singlearg",
                    std::function<void(int)>(
                        std::bind(&IDummy::dummy_void_singlearg, &md, _1)));
    raw_call(raw_msg);
}

TEST_F(binding_test, stdfunc_void_multiarg) {
    using namespace std::placeholders;
    MockDummy md;
    EXPECT_CALL(md, dummy_void_multiarg(363, 12));
    const unsigned char raw_msg[] = {0x94, 0x1,  0x0,  0xb3, 0x64, 0x75, 0x6d,
                                     0x6d, 0x79, 0x5f, 0x76, 0x6f, 0x69, 0x64,
                                     0x5f, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x61,
                                     0x72, 0x67, 0x92, 0xcd, 0x1,  0x6b, 0xc};
    dispatcher.bind("dummy_void_multiarg",
                    std::function<void(int, int)>(
                        std::bind(&IDummy::dummy_void_multiarg, &md, _1, _2)));
    raw_call(raw_msg);
}

TEST_F(dispatch_test, argcount_verified_void_nonzero_arg_too_few) {
    // raw_msg contains a call to dummy_void_singlearg but zero arguments
    const unsigned char raw_msg[] = {0x94, 0x1,  0x0,  0xb4, 0x64, 0x75, 0x6d,
                                     0x6d, 0x79, 0x5f, 0x76, 0x6f, 0x69, 0x64,
                                     0x5f, 0x73, 0x69, 0x6e, 0x67, 0x6c, 0x65,
                                     0x61, 0x72, 0x67, 0x90};
    dispatcher.bind("dummy_void_singlearg", &dummy_void_singlearg);
    EXPECT_THROW(raw_call(raw_msg), std::runtime_error);
    EXPECT_FALSE(g_dummy_void_singlearg_called);
}

TEST_F(dispatch_test, argcount_verified_void_nonzero_arg_too_many) {
    // raw_msg contains a call to dummy_void_singlearg but with two
    const unsigned char raw_msg[] = {0x94, 0x1,  0x0,  0xb4, 0x64, 0x75, 0x6d,
                                     0x6d, 0x79, 0x5f, 0x76, 0x6f, 0x69, 0x64,
                                     0x5f, 0x73, 0x69, 0x6e, 0x67, 0x6c, 0x65,
                                     0x61, 0x72, 0x67, 0x92, 0x1,  0x2};
    dispatcher.bind("dummy_void_singlearg", &dummy_void_singlearg);
    EXPECT_THROW(raw_call(raw_msg), std::runtime_error);
    EXPECT_FALSE(g_dummy_void_singlearg_called);
}

TEST_F(dispatch_test, unbound_func_error_response) {
    dispatcher.bind("foo", &dummy_void_singlearg);
    auto msg = make_unpacked(1, 0, "bar", msgpack::type::nil());
    auto response = dispatcher.dispatch(msg.get());
    EXPECT_TRUE(response.get_error().size() > 0);
}

