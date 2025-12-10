#include "CPluginController.hpp"
#include <cstdlib> // for atoi
#include <cstring>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
    // 参数校验
    if (argc != 2) {
        cout << "Usage:" << endl;
        cout << "  ./main help      : List all plugins" << endl;
        cout << "  ./main <ID>      : Execute plugin with specific ID" << endl;
        return 0;
    }

    CPluginController pc;

    // 模式 1: 查看帮助
    if (strcmp(argv[1], "help") == 0) {
        pc.ProcessHelp();
    }
    // 模式 2: 执行特定 ID 的功能
    else {
        int FunctionID = atoi(argv[1]);
        if (FunctionID == 0 && strcmp(argv[1], "0") != 0) {
            cout << "[Error] Invalid ID format." << endl;
            return 1;
        }

        // 初始化控制器（加载插件）
        if (pc.InitializeController()) {
            // 处理请求
            pc.ProcessRequest(FunctionID);
        }

        // UninitializeController 会在析构函数中自动调用
    }

    return 0;
}