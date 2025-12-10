#include <dlfcn.h> // 动态链接库接口头文件
#include <iostream>

using namespace std;

// 定义函数指针类型，用于指向动态库中的 Print 函数
typedef void (*FUNC_PRINT)();

int main() {
    // 1. 加载动态链接库
    // RTLD_LAZY: 暂缓决定，等有需要时再解出符号
    // 注意：如果在 CMake 构建目录下运行，库文件通常在当前目录
    void *handle = dlopen("./libfunc.so", RTLD_LAZY);

    // 检查是否加载成功
    if (handle == nullptr) {
        // dlerror() 返回具体的错误信息字符串
        cerr << "[Error] dlopen failed: " << dlerror() << endl;
        return 1;
    }

    // 2. 清除之前的错误信息
    dlerror();

    // 3. 获取函数地址
    // dlsym 返回 void*，需要强制转换为对应的函数指针类型
    FUNC_PRINT dl_print = (FUNC_PRINT)dlsym(handle, "Print");

    // 4. 检查是否找到符号
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        cerr << "[Error] dlsym failed: " << dlsym_error << endl;
        dlclose(handle); // 记得关闭句柄
        return 1;
    }

    // 5. 调用动态库中的函数
    cout << "Calling function from plugin..." << endl;
    dl_print();

    // 6. 卸载动态链接库
    dlclose(handle);

    return 0;
}