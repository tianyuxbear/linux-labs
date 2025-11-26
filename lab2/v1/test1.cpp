#include "A.hpp"

int main() {
    try {
        // 创建对象并初始化 i = 100
        A a(100);

        //========== 文件序列化与反序列化 ==========
        a.serialize("data1.bin"); // 写入二进制文件

        A b = A::deserialize("data1.bin"); // 从文件恢复对象

        // 美化输出：左侧标签固定宽度，保证对齐
        std::cout << "[From File] ";
        b.f(); // 输出反序列化得到的 i 的值

        //========== 缓冲区序列化与反序列化 ==========
        std::vector<char> buf = a.toBuffer(); // 写入内存 buffer

        A c = A::fromBuffer(buf); // 从 buffer 恢复对象

        std::cout << "[From Buffer] ";
        c.f(); // 输出从 buffer 恢复的结果
    } catch (const std::exception &e) {
        // 捕获所有异常并打印错误信息
        std::cerr << "发生错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
