#pragma once

// 定义类型标识常量，提高代码可读性
enum ObjectType {
    TYPE_A = 0,
    TYPE_B = 1
};

struct Serialized {
    int nType;  // 0 for A, 1 for B
    void *pObj; // 指向具体对象的指针 (在版本3中仍使用void*模拟多态前的阶段)
};