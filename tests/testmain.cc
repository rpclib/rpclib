#include "gtest/gtest.h"

int main(int argc, char *argv[]) {
    testing::FLAGS_gtest_color = "yes";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
