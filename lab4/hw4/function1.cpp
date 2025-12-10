#include "IPrintPlugin.hpp"
#include <iostream>

using namespace std;

const int FUNC_ID = 1;

// 1. 派生自抽象基类，实现具体功能
class CPrintPlugin : public IPrintPlugin {
public:
    CPrintPlugin() {}
    virtual ~CPrintPlugin() {}

    virtual void Print() override {
        cout << "Hello World!" << endl;
    }

    virtual void Help() override {
        cout << "Function ID " << FUNC_ID << " : This function will print hello world." << endl;
    }

    virtual int GetID() override {
        return FUNC_ID;
    }
};

// 2. 导出唯一的创建接口
// 注意：这里使用了 static 对象，这是一种简单的单例模式实现（符合 PPT 示例）
// 如果需要动态创建，可以使用 return new CPrintPlugin()，但需要在 Controller 中 delete
extern "C" void CreateObj(IPrintPlugin **ppPlugin) {
    static CPrintPlugin plugin;
    *ppPlugin = &plugin;
}