#pragma once
#include <iostream>

class B {
public:
    B() : i(0), j(0) {}
    B(int k) : i(k), j(k + 1) {}

    void f() const {
        std::cout << "[Class B] i = " << i << " , j = " << j << std::endl;
    }

    // 序列化：写入成员 i 和 j
    bool Serialize(std::ostream &os) const {
        os.write(reinterpret_cast<const char *>(&i), sizeof(i));
        os.write(reinterpret_cast<const char *>(&j), sizeof(j));
        return os.good();
    }

    // 反序列化：读取成员 i 和 j
    bool Deserialize(std::istream &is) {
        is.read(reinterpret_cast<char *>(&i), sizeof(i));
        is.read(reinterpret_cast<char *>(&j), sizeof(j));
        return is.good();
    }

private:
    int i, j;
};