#pragma once

#include <iostream>

// 不在头文件中使用 using namespace std; 以防止污染全局命名空间

class A {
private:
    int i;

public:
    A() : i(0) {} // 使用初始化列表

    A(int j) : i(j) {}

    // 序列化：将当前对象的成员 i 写入流中
    void Serialize(std::ostream &os) const {
        // 使用 reinterpret_cast 将 int 指针转为 char* 以便二进制写入
        os.write(reinterpret_cast<const char *>(&i), sizeof(i));
    }

    // 反序列化：从流中读取数据到当前对象的成员 i
    void Deserialize(std::istream &is) {
        is.read(reinterpret_cast<char *>(&i), sizeof(i));
    }

    void f() const {
        std::cout << "i = " << this->i << std::endl;
    }
};
