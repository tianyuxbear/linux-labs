/*************************************************************************
 * 文件名: server.cpp
 * 编程范式: 静态的面向对象的编程方法 - 模板/CRTP
 * 功能: 使用静态多态封装TCP服务端，实现Echo服务
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

// 模板基类：使用 CRTP (Curiously Recurring Template Pattern)
// T 是具体的子类类型
template <typename T>
class CTCPServer {
public:
    // 构造函数：使用 std::string 管理 IP 地址
    CTCPServer(int nServerPort, int nLengthOfQueueOfListen = 100, const char *strBoundIP = NULL) {
        m_nServerPort = nServerPort;
        m_nLengthOfQueueOfListen = nLengthOfQueueOfListen;

        if (NULL == strBoundIP) {
            m_strBoundIP = ""; // 空字符串代表 INADDR_ANY
        } else {
            m_strBoundIP = strBoundIP;
        }
    }

    // 静态多态通常不需要虚析构函数，因为我们通常直接使用子类对象，
    // 但为了安全起见，如果存在多态删除的需求，可以保留，这里为了纯粹展示静态多态特性，保持默认析构即可。
    ~CTCPServer() {
    }

public:
    int Run() {
        // 1. 创建 Socket
        int nListenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == nListenSocket) {
            std::cerr << "[Error] socket error" << std::endl;
            return -1;
        }

        // 2. 绑定地址
        sockaddr_in ServerAddress;
        memset(&ServerAddress, 0, sizeof(sockaddr_in));
        ServerAddress.sin_family = AF_INET;

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

        if (::bind(nListenSocket, (sockaddr *)&ServerAddress, sizeof(sockaddr_in)) == -1) {
            std::cerr << "[Error] bind error: " << strerror(errno) << std::endl;
            ::close(nListenSocket);
            return -1;
        }

        // 3. 监听
        if (::listen(nListenSocket, m_nLengthOfQueueOfListen) == -1) {
            std::cerr << "[Error] listen error" << std::endl;
            ::close(nListenSocket);
            return -1;
        }

        std::cout << "[Server] Listening on port " << m_nServerPort << "..." << std::endl;

        // 4. 循环处理连接
        while (true) {
            sockaddr_in ClientAddress;
            socklen_t LengthOfClientAddress = sizeof(sockaddr_in);

            int nConnectedSocket = ::accept(nListenSocket, (sockaddr *)&ClientAddress, &LengthOfClientAddress);
            if (-1 == nConnectedSocket) {
                std::cerr << "[Error] accept error" << std::endl;
                continue;
            }

            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &ClientAddress.sin_addr, clientIP, INET_ADDRSTRLEN);
            std::cout << "[Server] Client connected: " << clientIP << std::endl;

            // 5. 静态多态调用 (CRTP的核心)
            // 将 this 指针强制转换为子类指针 T*，并在编译期确定调用 T::ServerFunction
            T *pT = static_cast<T *>(this);
            pT->ServerFunction(nConnectedSocket, nListenSocket);

            ::close(nConnectedSocket);
            std::cout << "[Server] Client disconnected: " << clientIP << std::endl;
        }

        ::close(nListenSocket);
        return 0;
    }

    // 基类定义接口，但不实现，也不需要是 virtual
    void ServerFunction(int nConnectedSocket, int nListenSocket) {
        // 默认实现为空，或者抛出编译期错误
    }

private:
    int m_nServerPort;
    std::string m_strBoundIP;
    int m_nLengthOfQueueOfListen;
};

// 子类继承自 模板基类<子类>
class CMyTCPServer : public CTCPServer<CMyTCPServer> {
public:
    CMyTCPServer(int nServerPort, int nLengthOfQueueOfListen = 100, const char *strBoundIP = NULL)
        : CTCPServer<CMyTCPServer>(nServerPort, nLengthOfQueueOfListen, strBoundIP) {
    }

    ~CMyTCPServer() {
    }

    // 实现具体的业务逻辑 (Echo)
    // 注意：这里不需要 virtual 关键字
    void ServerFunction(int nConnectedSocket, int nListenSocket) {
        char buf[MAX_BUFFER_SIZE];
        while (true) {
            memset(buf, 0, MAX_BUFFER_SIZE);
            ssize_t bytesRead = ::read(nConnectedSocket, buf, MAX_BUFFER_SIZE - 1);

            if (bytesRead > 0) {
                std::cout << "[Recv]: " << buf;
                ssize_t bytesWrite = ::write(nConnectedSocket, buf, bytesRead); // Echo back
                assert(bytesRead == bytesWrite);
            } else {
                break; // Connection closed or error
            }
        }
    }
};

int main() {
    CMyTCPServer myserver(5000);
    myserver.Run();
    return 0;
}