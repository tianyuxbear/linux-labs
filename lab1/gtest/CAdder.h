#ifndef CADDER_H
#define CADDER_H

class CAdder {
public:
    CAdder();
    CAdder(int LeftPod, int RightPod);
    virtual ~CAdder();

public:
    // 方法1：直接传入两个数相加
    virtual int Add(int LeftPod, int RightPod);

    // 方法2：相加初始化时保存的两个数
    virtual int Add();

private:
    int m_LeftPod;
    int m_RightPod;
};

#endif