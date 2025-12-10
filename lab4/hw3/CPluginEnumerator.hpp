#pragma once

#include <string>
#include <vector>

using namespace std;

class CPluginEnumerator {
public:
    CPluginEnumerator();
    virtual ~CPluginEnumerator();

    bool GetPluginNames(vector<string> &vstrPluginNames);
};
