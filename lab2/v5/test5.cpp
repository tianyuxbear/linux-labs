#include "CLSerializer.hpp"
#include "Serializable.hpp"
#include <iostream>
#include <string>
#include <vector>


using namespace std;

// 【版本5新增功能】：读取配置文件获取序列化路径
// 使用现代 C++ 的 ifstream 和 string 替代 PPT 中的 fopen/fgets/char[]
string LoadConfig(const string &configFileName) {
    ifstream ifs(configFileName);
    if (!ifs.is_open()) {
        cerr << "Error: Could not open configuration file: " << configFileName << endl;
        return "";
    }

    string path;
    // 读取第一行作为路径，并自动处理去空格（>> 操作符特性）
    if (ifs >> path) {
        return path;
    }

    return "";
}

int main() {
    // 1. 读取配置
    const string CONFIG_FILE = "/home/xiongtianyu/uestc/linux/lab2/v5/config.txt";
    string targetPath = LoadConfig(CONFIG_FILE);

    if (targetPath.empty()) {
        cerr << "Failed to load target path from config. Exiting." << endl;
        return 1;
    }

    cout << "Configuration loaded. Target storage path: [" << targetPath << "]" << endl;

    // ================= 序列化过程 =================
    {
        cout << "\n--- Serializing ---" << endl;
        A a1(10);
        B b1(20);
        C c1(99.9);
        A a2(40);

        vector<ILSerializable *> v_write;
        v_write.push_back(&a1);
        v_write.push_back(&b1);
        v_write.push_back(&c1);
        v_write.push_back(&a2);

        CLSerializer s;
        // 使用配置中读取到的 targetPath
        if (s.Serialize(targetPath, v_write)) {
            cout << "Data successfully written to " << targetPath << endl;
        } else {
            cerr << "Failed to write data." << endl;
        }
    }

    // ================= 反序列化过程 =================
    {
        cout << "\n--- Deserializing ---" << endl;
        CLSerializer s;

        // 注册原型工厂
        A protoA;
        B protoB;
        C protoC;
        s.Register(&protoA);
        s.Register(&protoB);
        s.Register(&protoC);

        vector<unique_ptr<ILSerializable>> v_read;

        // 使用配置中读取到的 targetPath
        if (s.Deserialize(targetPath, v_read)) {
            cout << "Loaded " << v_read.size() << " objects from " << targetPath << ":" << endl;
            for (const auto &obj : v_read) {
                obj->f();
            }
        } else {
            cerr << "Deserialization failed. File might not exist or format is invalid." << endl;
        }
    }

    return 0;
}