#include "rpc/client.h"

#include <iostream>

int main() {
    rpc::client c("localhost", 8080);

    std::string text;
    while (std::getline(std::cin, text)) {
        if (!text.empty()) {
            std::string result(c.call("echo", text).as<std::string>());
            std::cout << "> " <<  result << std::endl;
        }
    }
}