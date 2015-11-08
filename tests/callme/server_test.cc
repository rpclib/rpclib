#include "gtest/gtest.h"

#include "callme/server.h"

unsigned dummy_unsigned(int x) {
    std::cout << x << std::endl;
    return 0;
}

void dummy_void(int x) { std::cout << "dummy_void!" << x << std::endl; }

TEST(server_functional, bind) {
    callme::server server("localhost", 8080);
    server.bind("dummy_unsigned", &dummy_unsigned);
    server.bind("dummy_void", &dummy_void);
}

TEST(server_msgpack, process_msg) {
    callme::server server("localhost", 8080);
    server.bind("dummy_void", &dummy_void);

    const unsigned char raw_msg[] = {0x94, 0x1,  0x0,  0xaa, 0x64,
                                     0x75, 0x6d, 0x6d, 0x79, 0x5f,
                                     0x76, 0x6f, 0x69, 0x64, 0x2a};
    msgpack::sbuffer msg;
    msg.write(reinterpret_cast<const char *>(raw_msg), sizeof(raw_msg));

    server.process_call(msg);
}
