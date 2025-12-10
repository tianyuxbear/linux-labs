#include "CPluginEnumerator.hpp"
#include <cstring> // for strcmp, strrchr
#include <dirent.h>
#include <iostream>
#include <string.h>

using namespace std;

CPluginEnumerator::CPluginEnumerator() {
}

CPluginEnumerator::~CPluginEnumerator() {
}

bool CPluginEnumerator::GetPluginNames(vector<string> &vstrPluginNames) {
    vstrPluginNames.clear();

    // 修改点：根据 PPT 要求，遍历当前目录下的 plugin 子目录
    const char *pluginDir = "./plugin";
    DIR *dir = opendir(pluginDir);

    if (dir == nullptr) {
        cerr << "[Error] Failed to open directory: " << pluginDir << endl;
        cerr << "Hint: Make sure the 'plugin' directory exists." << endl;
        return false;
    }

    struct dirent *pentry;
    // 循环读取目录项
    while ((pentry = readdir(dir)) != nullptr) {
        // 1. 跳过 "." (当前目录) 和 ".." (上级目录)
        if (strcmp(pentry->d_name, ".") == 0 || strcmp(pentry->d_name, "..") == 0) {
            continue;
        }

        // 2. 只收集以 .so 结尾的文件
        // strrchr 查找字符最后一次出现的位置
        const char *dot = strrchr(pentry->d_name, '.');
        if (!dot || strcmp(dot, ".so") != 0) {
            continue;
        }

        // 3. 拼接完整相对路径: ./plugin/libfunc.so
        string strPath = string(pluginDir) + "/" + string(pentry->d_name);

        vstrPluginNames.push_back(strPath);
    }

    closedir(dir);

    if (vstrPluginNames.empty()) {
        cout << "[Warning] No .so files found in " << pluginDir << endl;
        return false;
    }

    return true;
}