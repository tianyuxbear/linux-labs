#include "A.hpp"
#include "B.hpp"
#include "Serialized.hpp"
#include "Serializer.hpp"
#include <iostream>
#include <vector>

using namespace std;

int main() {
    const string DATA_FILE = "data3.bin";

    // ================= 序列化过程 =================
    {
        cout << "--- Start Serializing ---" << endl;
        A a1(2);
        B b1(3);
        B b2(4);
        A a2(5);

        // 构建 Serialized 结构体向量
        // 注意：这里手动指定类型 ID，这是版本3的特征
        vector<Serialized> v;
        v.push_back({TYPE_A, &a1});
        v.push_back({TYPE_B, &b1});
        v.push_back({TYPE_B, &b2});
        v.push_back({TYPE_A, &a2});

        if (Serializer::Serialize(DATA_FILE, v)) {
            cout << "Serialization success." << endl;
        } else {
            cerr << "Serialization failed." << endl;
            return 1;
        }
    }

    // ================= 反序列化过程 =================
    {
        cout << "\n--- Start Deserializing ---" << endl;
        vector<Serialized> v;

        if (Serializer::Deserialize(DATA_FILE, v)) {
            cout << "Deserialization success. Objects loaded: " << v.size() << endl;

            // 遍历并打印
            for (const auto &item : v) {
                if (item.nType == TYPE_A) {
                    A *p = static_cast<A *>(item.pObj);
                    p->f();
                } else if (item.nType == TYPE_B) {
                    B *p = static_cast<B *>(item.pObj);
                    p->f();
                }
            }

            // 【内存清理】重要：
            // 因为 Serializer::Deserialize 中使用了 new 创建对象，
            // 且 Serialized 结构体只持有 void*，无法自动析构，必须手动释放。
            // (这是版本3这种设计模式的固有缺陷，版本4引入多态基类后会更容易管理)
            for (auto &item : v) {
                if (item.nType == TYPE_A) {
                    delete static_cast<A *>(item.pObj);
                } else if (item.nType == TYPE_B) {
                    delete static_cast<B *>(item.pObj);
                }
            }
            v.clear();
            cout << "Memory cleaned up." << endl;
        } else {
            cerr << "Deserialization failed." << endl;
            return 1;
        }
    }

    return 0;
}