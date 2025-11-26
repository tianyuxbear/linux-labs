#pragma once
#include <iostream>
#include <memory> // for std::unique_ptr

// 抽象基类 / 接口
class ILSerializable {
public:
    virtual ~ILSerializable() = default;

    // 纯虚函数：序列化当前对象到输出流
    virtual bool Serialize(std::ostream &os) const = 0;

    // 纯虚函数：从输入流反序列化创建新对象 (原型模式工厂方法)
    // 返回 unique_ptr 以转移所有权，防止内存泄漏
    virtual std::unique_ptr<ILSerializable> Deserialize(std::istream &is) = 0;

    // 获取类型标识
    virtual int GetType() const = 0;

    // 辅助虚函数：用于打印信息（测试用）
    virtual void f() const = 0;
};

// ================= Class A =================
class A : public ILSerializable {
public:
    A() : i(0) {}
    explicit A(int val) : i(val) {}

    void f() const override {
        std::cout << "[Class A] i = " << i << std::endl;
    }

    int GetType() const override {
        return 0; // Type ID for A
    }

    bool Serialize(std::ostream &os) const override {
        os.write(reinterpret_cast<const char *>(&i), sizeof(i));
        return os.good();
    }

    std::unique_ptr<ILSerializable> Deserialize(std::istream &is) override {
        auto p = std::make_unique<A>();                        // 创建新对象
        is.read(reinterpret_cast<char *>(&(p->i)), sizeof(i)); // 填充数据
        return p;                                              // 返回基类指针
    }

private:
    int i;
};

// ================= Class B =================
class B : public ILSerializable {
public:
    B() : i(0), j(0) {}
    B(int val) : i(val), j(val + 1) {}

    void f() const override {
        std::cout << "[Class B] i = " << i << ", j = " << j << std::endl;
    }

    int GetType() const override {
        return 1; // Type ID for B
    }

    bool Serialize(std::ostream &os) const override {
        os.write(reinterpret_cast<const char *>(&i), sizeof(i));
        os.write(reinterpret_cast<const char *>(&j), sizeof(j));
        return os.good();
    }

    std::unique_ptr<ILSerializable> Deserialize(std::istream &is) override {
        auto p = std::make_unique<B>();
        is.read(reinterpret_cast<char *>(&(p->i)), sizeof(i));
        is.read(reinterpret_cast<char *>(&(p->j)), sizeof(j));
        return p;
    }

private:
    int i, j;
};

// ================= Class C  =================
class C : public ILSerializable {
public:
    C() : d(0.0) {}
    explicit C(double val) : d(val) {}

    void f() const override {
        std::cout << "[Class C] d = " << d << std::endl;
    }

    int GetType() const override {
        return 2; // Type ID for C
    }

    bool Serialize(std::ostream &os) const override {
        os.write(reinterpret_cast<const char *>(&d), sizeof(d));
        return os.good();
    }

    std::unique_ptr<ILSerializable> Deserialize(std::istream &is) override {
        auto p = std::make_unique<C>();
        is.read(reinterpret_cast<char *>(&(p->d)), sizeof(d));
        return p;
    }

private:
    double d;
};