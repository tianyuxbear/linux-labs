#pragma once

#include <vector>

// 定义函数指针类型
typedef void (*PROC_PRINT)(void);
typedef void (*PROC_HELP)(void);
typedef int (*PROC_GETID)(void);

class CPluginController {
public:
    CPluginController();
    virtual ~CPluginController(); // 析构函数中自动清理

    // 初始化：加载所有插件
    bool InitializeController();

    // 清理：卸载所有插件
    bool UninitializeController();

    // 模式1：调用所有插件的 Help 接口
    bool ProcessHelp();

    // 模式2：根据 ID 调用特定插件的 Print 接口
    bool ProcessRequest(int FunctionID);

private:
    std::vector<void *> m_vhForPlugin;    // 保存 dlopen 返回的句柄
    std::vector<PROC_PRINT> m_vPrintFunc; // 保存 Print 函数地址
    std::vector<PROC_GETID> m_vGetIDFunc; // 保存 GetID 函数地址
};
