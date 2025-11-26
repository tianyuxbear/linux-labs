#include <gtest/gtest.h>

int main(int argc, char *argv[]) {
    // 初始化 GoogleTest
    testing::InitGoogleTest(&argc, argv);

    // 运行所有注册的测试用例
    return RUN_ALL_TESTS();
}