#pragma once

#include "Serializable.hpp"
#include <fstream>
#include <string>
#include <vector>

class CLSerializer {
public:
    // 序列化：将对象列表写入文件
    // 参数使用 const 引用，避免拷贝
    bool Serialize(const std::string &filePath, const std::vector<ILSerializable *> &v) {
        std::ofstream ofs(filePath, std::ios::binary);
        if (!ofs.is_open()) {
            return false;
        }

        for (const auto *ptr : v) {
            if (!ptr) {
                continue;
            }

            int type = ptr->GetType();
            // 1. 写入类型 ID
            ofs.write(reinterpret_cast<const char *>(&type), sizeof(int));
            // 2. 调用对象自身的序列化方法
            ptr->Serialize(ofs);
        }
        return true; // ofstream 会自动关闭
    }

    // 反序列化：从文件读取并重建对象列表
    bool Deserialize(const std::string &filePath, std::vector<std::unique_ptr<ILSerializable>> &v) {
        std::ifstream ifs(filePath, std::ios::binary);
        if (!ifs.is_open()) {
            return false;
        }

        // 尝试读取文件直到结束
        while (ifs.peek() != EOF) {
            int nType = -1;
            // 1. 读取类型 ID
            ifs.read(reinterpret_cast<char *>(&nType), sizeof(int));

            if (ifs.eof() || ifs.fail()) {
                break;
            }

            // 2. 在注册的原型列表中查找对应的处理器
            bool found = false;
            for (auto *proto : m_prototypes) {
                if (proto->GetType() == nType) {
                    // 3. 调用原型的 Deserialize 创建新对象
                    // 原型工厂模式的核心：proto 是工厂，返回的是新创建的 unique_ptr
                    auto newObj = proto->Deserialize(ifs);
                    if (newObj) {
                        v.push_back(std::move(newObj)); // 转移所有权到 vector
                        found = true;
                    }
                    break;
                }
            }

            if (!found) {
                std::cerr << "Warning: Unknown type ID " << nType << " encountered." << std::endl;
                // 在实际项目中，这里可能需要一种机制来跳过未知对象的字节，或者直接报错
                return false;
            }
        }
        return true;
    }

    // 注册原型对象
    void Register(ILSerializable *pSerialized) {
        m_prototypes.push_back(pSerialized);
    }

private:
    // 存储用于反序列化的“原型”对象指针
    std::vector<ILSerializable *> m_prototypes;
};