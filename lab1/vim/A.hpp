#pragma once

#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

class A {
public:
    A() = default;
    explicit A(int j) : i(j) {}

    // 将对象中的成员 i 以二进制形式写入指定文件
    void serialize(const std::string &filename) const {
        // 以二进制写模式打开文件
        std::ofstream os(filename, std::ios::binary);
        if (!os) {
            throw std::runtime_error("无法打开文件进行写入");
        }

        // 直接写入 i 的 4 字节原始二进制数据
        os.write(reinterpret_cast<const char *>(&i), sizeof(i));
        if (!os) {
            throw std::runtime_error("写入文件失败");
        }
    }

    // 从指定的二进制文件中读取数据并恢复对象
    static A deserialize(const std::string &filename) {
        // 以二进制读模式打开文件
        std::ifstream is(filename, std::ios::binary);
        if (!is) {
            throw std::runtime_error("无法打开文件进行读取");
        }

        A a;
        // 读取 4 字节的成员 i
        is.read(reinterpret_cast<char *>(&a.i), sizeof(a.i));
        if (!is) {
            throw std::runtime_error("从文件读取数据失败");
        }

        return a;
    }

    // 将成员 i 的二进制内容写入到内存缓冲区（vector<char>）
    std::vector<char> toBuffer() const {
        std::vector<char> buffer(sizeof(i));
        // 内存拷贝：将 i 的原始字节拷贝到 buffer 中
        std::memcpy(buffer.data(), &i, sizeof(i));
        return buffer;
    }

    // 从内存缓冲区恢复对象（缓冲区必须至少包含 sizeof(int) 字节）
    static A fromBuffer(const std::vector<char> &buffer) {
        if (buffer.size() != sizeof(int)) {
            throw std::runtime_error("缓冲区大小不符合要求");
        }

        A a;
        // 从缓冲区中恢复成员 i
        std::memcpy(&a.i, buffer.data(), sizeof(a.i));
        return a;
    }

    // 打印当前对象的成员 i
    void f() const { std::cout << "i = " << i << std::endl; }

private:
    int i{0}; // 需要被序列化的成员
};
