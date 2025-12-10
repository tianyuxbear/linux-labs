#include "CPluginEnumerator.hpp"
#include <dlfcn.h>
#include <iostream>
#include <vector>

using namespace std;

// 定义函数指针类型
typedef void (*FUNC_PRINT)();

int main() {
    vector<string> vstrPluginNames;
    CPluginEnumerator enumerator;

    // 1. 获取插件列表
    if (!enumerator.GetPluginNames(vstrPluginNames)) {
        return 1;
    }

    cout << "Found " << vstrPluginNames.size() << " plugins." << endl;

    // 2. 遍历加载每一个插件
    for (size_t i = 0; i < vstrPluginNames.size(); i++) {
        cout << "--------------------------------" << endl;
        cout << "Loading: " << vstrPluginNames[i] << endl;

        // 加载动态库
        void *handle = dlopen(vstrPluginNames[i].c_str(), RTLD_LAZY);
        if (handle == nullptr) {
            cerr << "[Error] dlopen failed: " << dlerror() << endl;
            continue; // 跳过当前插件，尝试下一个
        }

        // 获取 Print 函数地址
        FUNC_PRINT dl_print = (FUNC_PRINT)dlsym(handle, "Print");
        const char *dlsym_error = dlerror();
        if (dlsym_error) {
            cerr << "[Error] dlsym failed: " << dlsym_error << endl;
            dlclose(handle);
            continue;
        }

        // 执行函数
        dl_print();

        // 卸载动态库
        dlclose(handle);
    }
    cout << "--------------------------------" << endl;

    return 0;
}