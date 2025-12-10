#include <iostream>

using namespace std;

// extern "C" 告诉 C++ 编译器按照 C 语言的方式编译该函数
// 这样函数名不会被修饰（Name Mangling），dlsym 才能通过 "Print" 找到它
extern "C" void Print() {
    // 实验步骤：
    // 1. 初次编译时，这里可以写 "Hello World!"
    // cout << "Hello World!" << endl;
    // 2. 验证“插件更新”时，修改为 "Hello China!" 并重新编译库，无需重新编译 main
    cout << "Hello China!" << endl;
}