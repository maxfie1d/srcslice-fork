#include <gtest/gtest.h>

namespace {
    class MyTest : public ::testing::Test {
    };

    TEST_F(MyTest, isOdd) {
        std::cout << "テストをするよ！" << std::endl;
        EXPECT_EQ(3, 1 + 2);
    }
}
