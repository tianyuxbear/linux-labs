#include "CPluginController.hpp"
#include "CPluginEnumerator.hpp"
#include <dlfcn.h>
#include <iostream>

using namespace std;

CPluginController::CPluginController() {}

CPluginController::~CPluginController() {
    // 确保对象销毁时释放动态库资源
    UninitializeController();
}

bool CPluginController::InitializeController() {
    vector<string> vstrPluginNames;
    CPluginEnumerator enumerator;

    // 1. 获取插件文件列表
    if (!enumerator.GetPluginNames(vstrPluginNames)) {
        cout << "[Info] No plugins found." << endl;
        return false;
    }

    // 2. 遍历加载
    for (const auto &path : vstrPluginNames) {
        // 加载动态库
        void *hinstLib = dlopen(path.c_str(), RTLD_LAZY);
        if (hinstLib == nullptr) {
            cerr << "[Error] dlopen failed for " << path << ": " << dlerror() << endl;
            continue;
        }

        // 获取 Print 和 GetID 函数地址
        PROC_PRINT DllPrint = (PROC_PRINT)dlsym(hinstLib, "Print");
        PROC_GETID DllGetID = (PROC_GETID)dlsym(hinstLib, "GetID");

        // 校验：必须两个函数都存在才算加载成功
        if (DllPrint && DllGetID) {
            m_vhForPlugin.push_back(hinstLib);
            m_vPrintFunc.push_back(DllPrint);
            m_vGetIDFunc.push_back(DllGetID);
        } else {
            cerr << "[Error] Missing symbols in " << path << ": " << dlerror() << endl;
            dlclose(hinstLib);
        }
    }
    return true;
}

bool CPluginController::ProcessRequest(int FunctionID) {
    bool bFound = false;
    // 遍历已加载的 GetID 函数，寻找匹配的 ID
    for (size_t i = 0; i < m_vGetIDFunc.size(); i++) {
        if (m_vGetIDFunc[i]() == FunctionID) {
            // 找到 ID，调用对应的 Print
            m_vPrintFunc[i]();
            bFound = true;
            break;
        }
    }

    if (!bFound) {
        cout << "[Warning] Function ID " << FunctionID << " not found." << endl;
    }

    return true;
}

bool CPluginController::ProcessHelp() {
    vector<string> vstrPluginNames;
    CPluginEnumerator enumerator;

    if (!enumerator.GetPluginNames(vstrPluginNames)) {
        return false;
    }

    // Help 模式下，临时加载调用后立即释放，不需要常驻内存
    for (const auto &path : vstrPluginNames) {
        void *hinstLib = dlopen(path.c_str(), RTLD_LAZY);
        if (hinstLib != nullptr) {
            PROC_HELP DllHelp = (PROC_HELP)dlsym(hinstLib, "Help");
            if (DllHelp) {
                DllHelp(); // 调用 Help
            }
            dlclose(hinstLib);
        }
    }
    return true;
}

bool CPluginController::UninitializeController() {
    // 释放所有持有的动态库句柄
    for (void *handle : m_vhForPlugin) {
        if (handle) {
            dlclose(handle);
        }
    }
    m_vhForPlugin.clear();
    m_vPrintFunc.clear();
    m_vGetIDFunc.clear();
    return true;
}