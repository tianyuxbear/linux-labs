#include "CAdder.h"
#include <gtest/gtest.h>

// 测试用例 1：测试带参数的 Add 方法 (基础正数)
// 修正了原代码中 3+4=8 的错误预期
TEST(CAdderTest, AddWithArgs_PositiveNumbers) {
    CAdder adder;
    // 3 + 4 应该等于 7
    EXPECT_EQ(7, adder.Add(3, 4));
}

// 测试用例 2：测试无参数 Add 方法 (基于构造函数状态)
TEST(CAdderTest, AddWithoutArgs_ConstructorInit) {
    CAdder adder(3, 4);
    // 成员变量 3 + 4 应该等于 7
    EXPECT_EQ(7, adder.Add());
}

// 测试用例 3：测试负数处理 (新增用例)
TEST(CAdderTest, HandleNegativeNumbers) {
    CAdder adder;
    // -5 + 10 应该等于 5
    EXPECT_EQ(5, adder.Add(-5, 10));

    // -5 + -5 应该等于 -10
    EXPECT_EQ(-10, adder.Add(-5, -5));
}

// 测试用例 4：测试零值处理 (新增用例)
TEST(CAdderTest, HandleZero) {
    CAdder adder(0, 5);
    EXPECT_EQ(5, adder.Add());
    EXPECT_EQ(0, adder.Add(0, 0));
}