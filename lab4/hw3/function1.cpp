#include <iostream>

using namespace std;

const int FUNC_ID = 1;

extern "C" {
// 业务功能
void Print() {
    cout << "Hello World!" << endl;
}

// 辅助功能：显示帮助信息
void Help() {
    cout << "Function ID " << FUNC_ID << " : This function will print hello world." << endl;
}

// 辅助功能：获取插件 ID
int GetID() {
    return FUNC_ID;
}
}