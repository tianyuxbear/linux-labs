#pragma once
#include <iostream>

class A {
public:
    A() : i(0) {}
    A(int j) : i(j) {}

    void f() const {
        std::cout << "[Class A] i = " << i << std::endl;
    }

    // 序列化：写入成员 i
    bool Serialize(std::ostream &os) const {
        os.write(reinterpret_cast<const char *>(&i), sizeof(i));
        return os.good();
    }

    // 反序列化：读取成员 i
    bool Deserialize(std::istream &is) {
        is.read(reinterpret_cast<char *>(&i), sizeof(i));
        return is.good();
    }

private:
    int i;
};