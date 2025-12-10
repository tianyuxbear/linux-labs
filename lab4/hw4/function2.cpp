#include "IPrintPlugin.hpp"
#include <iostream>

using namespace std;

const int FUNC_ID = 2;

class CPrintPlugin : public IPrintPlugin {
public:
    CPrintPlugin() {}
    virtual ~CPrintPlugin() {}

    virtual void Print() override {
        cout << "Hello China!" << endl;
    }

    virtual void Help() override {
        cout << "Function ID " << FUNC_ID << " : This function will print hello china." << endl;
    }

    virtual int GetID() override {
        return FUNC_ID;
    }
};

extern "C" void CreateObj(IPrintPlugin **ppPlugin) {
    static CPrintPlugin plugin;
    *ppPlugin = &plugin;
}