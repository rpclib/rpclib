#include <iostream>
#include "callme/client.h"

int main() {
    callme::client client("127.0.0.1", 8080);
    client.run();
    auto result = client.call("sum", 2, 3);
    int result_value = -1;
    result.convert(&result_value);
    std::cout << "The result is: " << result_value << std::endl;
    return 0;
}
