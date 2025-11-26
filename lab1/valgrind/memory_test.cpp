#include <iostream>

void faulty_function() {
    // 申请 10 个 int 的空间 (有效索引范围是 0 到 9)
    int *array = new int[10];

    std::cout << "[Info] Memory allocated." << std::endl;

    // 错误 1: 堆越界写入 (Invalid Write)
    // 试图写入第 11 个元素 (下标 10)，导致越界
    array[10] = 2024;

    std::cout << "[Info] Out-of-bounds write performed." << std::endl;

    // 错误 2: 内存泄漏 (Memory Leak)
    // 忘记调用 delete[] array;
}

int main() {
    std::cout << "--- Starting Valgrind Test ---" << std::endl;
    faulty_function();
    std::cout << "--- End of Program ---" << std::endl;
    return 0;
}