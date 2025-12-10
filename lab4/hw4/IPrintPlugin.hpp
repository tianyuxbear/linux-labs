#pragma once

class IPrintPlugin {
public:
    IPrintPlugin();
    // 务必声明虚析构函数，保证派生类能被正确销毁
    virtual ~IPrintPlugin();

    virtual void Help() = 0;
    virtual void Print() = 0;
    virtual int GetID() = 0;
};
