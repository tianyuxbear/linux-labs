#pragma once

#include "A.hpp"
#include <fstream>
#include <iostream>
#include <vector>

class SerializerForAs {
public:
    // 序列化：将 vector 中的所有 A 对象写入文件
    static bool Serialize(const std::string &pFilePath, const std::vector<A *> &v) {
        // 以二进制模式打开文件
        std::ofstream ofs(pFilePath, std::ios::binary);
        if (!ofs.is_open()) {
            std::cerr << "Error: Unable to open file for writing: " << pFilePath << std::endl;
            return false;
        }

        // 遍历 vector，调用每个对象的 Serialize 方法
        for (const auto *ptr : v) {
            if (ptr != nullptr) {
                ptr->Serialize(ofs);
            }
        }

        ofs.close();
        return true;
    }

    // 反序列化：从文件读取数据填充到 vector 中的对象里
    // 注意：vector 已经被填充了对象实例，只是去覆盖它们的值
    static bool Deserialize(const std::string &pFilePath, const std::vector<A *> &v) {
        std::ifstream ifs(pFilePath, std::ios::binary);
        if (!ifs.is_open()) {
            std::cerr << "Error: Unable to open file for reading: " << pFilePath << std::endl;
            return false;
        }

        // 遍历 vector，从文件中读取数据填入每个对象
        for (auto *ptr : v) {
            if (ptr != nullptr && ifs.good()) {
                ptr->Deserialize(ifs);
            }
        }

        ifs.close();
        return true;
    }
};
