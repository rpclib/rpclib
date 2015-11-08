#include "gtest/gtest.h"

#include "callme/server.h"

unsigned dummy_unsigned(int x) { std::cout << x << std::endl; return 0; }

void dummy_void(int x) { std::cout << x << std::endl; }

TEST(server_functional, bind) {
  callme::server server("localhost", 8080);
  server.bind("dummy_unsigned", &dummy_unsigned);
  server.bind("dummy_void", &dummy_void);
}
