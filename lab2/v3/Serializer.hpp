#pragma once

#include "A.hpp"
#include "B.hpp"
#include "Serialized.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Serializer {
public:
    // 将对象列表写入文件
    static bool Serialize(const std::string &filePath, const std::vector<Serialized> &v) {
        std::ofstream ofs(filePath, std::ios::binary);
        if (!ofs.is_open()) {
            return false;
        }

        for (const auto &item : v) {
            // 1. 先写入类型标记
            ofs.write(reinterpret_cast<const char *>(&item.nType), sizeof(int));

            // 2. 根据类型强制转换并调用对应的序列化
            if (item.nType == TYPE_A) {
                A *p = static_cast<A *>(item.pObj);
                p->Serialize(ofs);
            } else if (item.nType == TYPE_B) {
                B *p = static_cast<B *>(item.pObj);
                p->Serialize(ofs);
            }
        }
        ofs.close();
        return true;
    }

    // 从文件读取对象列表
    static bool Deserialize(const std::string &filePath, std::vector<Serialized> &v) {
        std::ifstream ifs(filePath, std::ios::binary);
        if (!ifs.is_open()) {
            return false;
        }

        while (true) {
            int nType = -1;
            // 1. 尝试读取类型标记
            ifs.read(reinterpret_cast<char *>(&nType), sizeof(int));

            // 如果读取结束或出错，退出循环
            if (ifs.eof() || !ifs.good()) {
                break;
            }

            Serialized s;
            s.nType = nType;
            s.pObj = nullptr;

            // 2. 根据类型创建对象并反序列化
            if (nType == TYPE_A) {
                A *p = new A();
                if (p->Deserialize(ifs)) {
                    s.pObj = p;
                } else {
                    delete p; // 读取失败防止内存泄漏
                    break;
                }
            } else if (nType == TYPE_B) {
                B *p = new B();
                if (p->Deserialize(ifs)) {
                    s.pObj = p;
                } else {
                    delete p;
                    break;
                }
            }

            if (s.pObj != nullptr) {
                v.push_back(s);
            }
        }
        ifs.close();
        return true;
    }
};