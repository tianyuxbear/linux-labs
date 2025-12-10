#include "CPluginController.hpp"
#include "CPluginEnumerator.hpp"
#include "IPrintPlugin.hpp"
#include <dlfcn.h>
#include <iostream>

using namespace std;

// 定义创建对象的函数指针类型
typedef void (*PLUGIN_CREATE)(IPrintPlugin **);

CPluginController::CPluginController() {
}

CPluginController::~CPluginController() {
    UninitializeController();
}

bool CPluginController::InitializeController() {
    vector<string> vstrPluginNames;
    CPluginEnumerator enumerator;

    if (!enumerator.GetPluginNames(vstrPluginNames)) {
        return false;
    }

    for (const auto &path : vstrPluginNames) {
        // 1. 加载动态库
        void *hinstLib = dlopen(path.c_str(), RTLD_LAZY);
        if (hinstLib == nullptr) {
            cerr << "[Error] dlopen failed: " << dlerror() << endl;
            continue;
        }

        // 2. 获取 CreateObj 函数地址
        PLUGIN_CREATE CreateProc = (PLUGIN_CREATE)dlsym(hinstLib, "CreateObj");

        // 3. 创建对象
        if (CreateProc != nullptr) {
            IPrintPlugin *pPlugin = nullptr;
            (CreateProc)(&pPlugin); // 调用插件的工厂函数

            if (pPlugin != nullptr) {
                // 成功：保存句柄和对象指针
                m_vhForPlugin.push_back(hinstLib);
                m_vpPlugin.push_back(pPlugin);
            } else {
                dlclose(hinstLib);
            }
        } else {
            cerr << "[Error] CreateObj not found in " << path << endl;
            dlclose(hinstLib);
        }
    }

    return true;
}

bool CPluginController::ProcessRequest(int FunctionID) {
    bool bFound = false;
    // 遍历对象列表
    for (auto *plugin : m_vpPlugin) {
        if (plugin->GetID() == FunctionID) {
            plugin->Print(); // 多态调用
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
    // 注意：根据 main.cpp 的逻辑，调用 Help 时并未调用 InitializeController
    // 因此这里需要独立完成 加载->调用Help->卸载 的过程
    vector<string> vstrPluginNames;
    CPluginEnumerator enumerator;

    if (!enumerator.GetPluginNames(vstrPluginNames)) {
        return false;
    }

    for (const auto &path : vstrPluginNames) {
        void *hinstLib = dlopen(path.c_str(), RTLD_LAZY);
        if (hinstLib != nullptr) {
            PLUGIN_CREATE CreateProc = (PLUGIN_CREATE)dlsym(hinstLib, "CreateObj");
            if (CreateProc != nullptr) {
                IPrintPlugin *pPlugin = nullptr;
                (CreateProc)(&pPlugin);

                if (pPlugin != nullptr) {
                    pPlugin->Help(); // 多态调用 Help
                }
            }
            // 用完即刻销毁
            dlclose(hinstLib);
        }
    }
    return true;
}

bool CPluginController::UninitializeController() {
    // 释放所有动态库句柄
    for (void *handle : m_vhForPlugin) {
        if (handle) {
            dlclose(handle);
        }
    }
    m_vhForPlugin.clear();
    m_vpPlugin.clear();
    return true;
}