#include "gtest/gtest.h"

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
	std::cin.ignore();
    return RUN_ALL_TESTS();
}
