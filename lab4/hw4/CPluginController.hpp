#pragma once

#include <vector>

// 前向声明，减少头文件依赖
class IPrintPlugin;

class CPluginController {
public:
    CPluginController();
    virtual ~CPluginController();

    bool InitializeController();
    bool UninitializeController();

    bool ProcessHelp();
    bool ProcessRequest(int FunctionID);

private:
    // 保存动态库句柄，用于释放资源
    std::vector<void *> m_vhForPlugin;
    // 保存插件对象指针，用于调用功能
    std::vector<IPrintPlugin *> m_vpPlugin;
};
