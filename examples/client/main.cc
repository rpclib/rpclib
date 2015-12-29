#include <iostream>
#include "callme/client.h"

int main() {
    callme::client client("127.0.0.1", 8080);
    client.run();
    std::cin.ignore();
    client.call("sum", 2, 3);
    std::cin.ignore();
}
