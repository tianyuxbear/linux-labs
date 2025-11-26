#include "CLSerializer.hpp"
#include "Serializable.hpp"
#include <iostream>
#include <vector>

using namespace std;

int main() {
    const string DATA_FILE = "data4.bin";

    // ================= 序列化过程 =================
    {
        cout << "--- Serializing ---" << endl;
        A a1(10);
        B b1(20);
        C c1(3.14); // 新增类 C 的对象
        B b2(30);
        A a2(40);

        // 使用原始指针存入 vector 用于序列化（因为对象在栈上，不用 unique_ptr）
        vector<ILSerializable *> v_write;
        v_write.push_back(&a1);
        v_write.push_back(&b1);
        v_write.push_back(&c1);
        v_write.push_back(&b2);
        v_write.push_back(&a2);

        CLSerializer s;
        if (s.Serialize(DATA_FILE, v_write)) {
            cout << "Data written to " << DATA_FILE << endl;
        }
    }

    // ================= 反序列化过程 =================
    {
        cout << "\n--- Deserializing ---" << endl;
        CLSerializer s;

        // 1. 注册原型 (Prototype Registration)
        // 这些对象本身不存储数据，只作为工厂使用
        A protoA;
        B protoB;
        C protoC;
        s.Register(&protoA);
        s.Register(&protoB);
        s.Register(&protoC);

        // 2. 接收容器
        // 使用 unique_ptr 自动管理内存，无需手动 delete
        vector<unique_ptr<ILSerializable>> v_read;

        if (s.Deserialize(DATA_FILE, v_read)) {
            cout << "Loaded " << v_read.size() << " objects." << endl;

            // 3. 遍历并验证多态
            for (const auto &obj : v_read) {
                // 直接调用虚函数 f()，无需 dynamic_cast
                // 如果需要特定类型的操作，仍然可以使用 dynamic_cast
                obj->f();
            }
        } else {
            cerr << "Deserialization failed." << endl;
        }

        // v_read 析构时，所有 unique_ptr 自动释放内存
        cout << "Memory automatically cleaned up." << endl;
    }

    return 0;
}