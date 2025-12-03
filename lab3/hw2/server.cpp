/*************************************************************************
 * 文件名: server.cpp
 * 编程范式: 面向对象的编程方法
 * 功能: 封装TCP服务端类，通过继承和多态实现Echo服务器
 *************************************************************************/
#include <arpa/inet.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024
#define DEFAULT_PORT 5000

// 基类：CTCPServer
// 负责底层的Socket创建、绑定、监听和连接接受
class CTCPServer {
public:
    // 构造函数：初始化端口、监听队列长度和绑定IP
    CTCPServer(int nServerPort, int nLengthOfQueueOfListen = 100, const char *strBoundIP = NULL) {
        m_nServerPort = nServerPort;
        m_nLengthOfQueueOfListen = nLengthOfQueueOfListen;

        // 使用 std::string 自动管理内存，避免手动 new/delete
        if (NULL == strBoundIP) {
            m_strBoundIP = ""; // 空字符串代表 INADDR_ANY
        } else {
            m_strBoundIP = strBoundIP;
        }
    }

    // 虚析构函数：确保派生类能正确释放资源
    virtual ~CTCPServer() {
    }

public:
    // 核心运行逻辑
    int Run() {
        // 1. 创建监听套接字
        int nListenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == nListenSocket) {
            std::cerr << "[Error] socket error" << std::endl;
            return -1;
        }

        // 2. 初始化地址结构
        sockaddr_in ServerAddress;
        memset(&ServerAddress, 0, sizeof(sockaddr_in));
        ServerAddress.sin_family = AF_INET;

        // 处理 IP 绑定
        if (m_strBoundIP.empty()) {
            ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        } else {
            if (::inet_pton(AF_INET, m_strBoundIP.c_str(), &ServerAddress.sin_addr) != 1) {
                std::cerr << "[Error] inet_pton error" << std::endl;
                ::close(nListenSocket);
                return -1;
            }
        }

        ServerAddress.sin_port = htons(m_nServerPort);

        // 3. 绑定端口 (Bind)
        if (::bind(nListenSocket, (sockaddr *)&ServerAddress, sizeof(sockaddr_in)) == -1) {
            std::cerr << "[Error] bind error: " << strerror(errno) << std::endl;
            ::close(nListenSocket);
            return -1;
        }

        // 4. 监听端口 (Listen)
        if (::listen(nListenSocket, m_nLengthOfQueueOfListen) == -1) {
            std::cerr << "[Error] listen error" << std::endl;
            ::close(nListenSocket);
            return -1;
        }

        std::cout << "[Server] Listening on port " << m_nServerPort << "..." << std::endl;

        // 5. 循环处理客户端连接
        while (true) {
            sockaddr_in ClientAddress;
            socklen_t LengthOfClientAddress = sizeof(sockaddr_in);

            // 阻塞等待连接
            int nConnectedSocket = ::accept(nListenSocket, (sockaddr *)&ClientAddress, &LengthOfClientAddress);
            if (-1 == nConnectedSocket) {
                std::cerr << "[Error] accept error" << std::endl;
                continue; // 继续等待下一个连接
            }

            // 打印客户端信息
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &ClientAddress.sin_addr, clientIP, INET_ADDRSTRLEN);
            std::cout << "[Server] Client connected: " << clientIP << std::endl;

            // 6. 多态调用：调用派生类实现的具体业务逻辑
            ServerFunction(nConnectedSocket, nListenSocket);

            // 业务处理完毕，关闭连接
            ::close(nConnectedSocket);
            std::cout << "[Server] Client disconnected: " << clientIP << std::endl;
        }

        ::close(nListenSocket);
        return 0;
    }

private:
    // 纯虚函数或虚函数：由派生类实现具体业务
    // 对应 PPT 中 3.14 节的类图结构
    virtual void ServerFunction(int nConnectedSocket, int nListenSocket) {
        // 基类中不做具体处理
    }

private:
    int m_nServerPort;
    std::string m_strBoundIP; // 使用 string 替代 char*
    int m_nLengthOfQueueOfListen;
};

// 派生类：CMyTCPServer
// 继承自 CTCPServer，重写 ServerFunction 实现 Echo 业务
class CMyTCPServer : public CTCPServer {
public:
    CMyTCPServer(int nServerPort, int nLengthOfQueueOfListen = 100, const char *strBoundIP = NULL)
        : CTCPServer(nServerPort, nLengthOfQueueOfListen, strBoundIP) {
    }

    virtual ~CMyTCPServer() {
    }

private:
    // 重写父类的虚函数，实现 Echo 服务逻辑
    virtual void ServerFunction(int nConnectedSocket, int nListenSocket) {
        char buf[MAX_BUFFER_SIZE];

        while (true) {
            memset(buf, 0, MAX_BUFFER_SIZE);
            // 读取客户端数据
            ssize_t bytesRead = ::read(nConnectedSocket, buf, MAX_BUFFER_SIZE - 1);

            if (bytesRead > 0) {
                std::cout << "[Recv]: " << buf;
                // 将接收到的数据原样发回 (Echo)
                ssize_t bytesWrite = ::write(nConnectedSocket, buf, bytesRead);
                assert(bytesRead == bytesWrite);
            } else if (bytesRead == 0) {
                // 客户端关闭连接
                break;
            } else {
                std::cerr << "[Error] read error" << std::endl;
                break;
            }
        }
    }
};

int main() {
    // 实例化派生类对象
    CMyTCPServer myserver(5000);
    // 调用基类的 Run 方法，Run 方法内部会多态调用 ServerFunction
    myserver.Run();
    return 0;
}