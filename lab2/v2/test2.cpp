#include "A.hpp"
#include "SerializerForAs.hpp"
#include <iostream>
#include <vector>

using namespace std;

int main() {
    // 1. 准备数据
    A a1(100);
    A a2(200);

    vector<A *> v1;
    v1.push_back(&a1);
    v1.push_back(&a2);

    // 2. 序列化写入文件
    // 此时文件 data2.bin 中将包含 8 个字节 (两个 int)
    if (SerializerForAs::Serialize("data2.bin", v1)) {
        cout << "Serialization successful." << endl;
    }

    // 3. 准备接收对象
    A a3; // 初始为 0
    A a4; // 初始为 0

    vector<A *> v2;
    v2.push_back(&a3);
    v2.push_back(&a4);

    // 4. 反序列化读取文件
    // a3 将变成 100, a4 将变成 200
    if (SerializerForAs::Deserialize("data2.bin", v2)) {
        cout << "Deserialization successful." << endl;
    }

    // 5. 验证结果
    cout << "Results:" << endl;
    a3.f(); // 预期输出: i = 100
    a4.f(); // 预期输出: i = 200

    return 0;
}