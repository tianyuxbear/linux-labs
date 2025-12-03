/*************************************************************************
 * 文件名: server.cpp
 * 编程范式: 基于接口的编程方法
 * 功能: 封装TCP服务端通信库，通过Observer接口分离业务逻辑，实现Echo服务
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

// -----------------------------------------------------------
// 1. 定义接口类 (Observer Interface)
// -----------------------------------------------------------
class CTCPServerObserver {
public:
    CTCPServerObserver() {}
    virtual ~CTCPServerObserver() {}

public:
    // 纯虚函数：定义服务端业务逻辑的规范
    // 当连接建立成功后，通信库会回调此函数
    virtual void ServerFunction(int nConnectedSocket, int nListenSocket) = 0;
};

// -----------------------------------------------------------
// 2. 定义通信库类 (Network Wrapper)
// -----------------------------------------------------------
class CTCPServer {
public:
    // 构造函数：接受一个观察者对象指针，实现依赖注入
    CTCPServer(CTCPServerObserver *pObserver, int nServerPort, int nLengthOfQueueOfListen = 100, const char *strBoundIP = NULL) {
        m_pObserver = pObserver; // 保存接口指针
        m_nServerPort = nServerPort;
        m_nLengthOfQueueOfListen = nLengthOfQueueOfListen;

        if (NULL == strBoundIP) {
            m_strBoundIP = ""; // 空字符串代表 INADDR_ANY
        } else {
            m_strBoundIP = strBoundIP;
        }
    }

    virtual ~CTCPServer() {
        // m_pObserver 的生命周期通常由外部管理，这里不负责 delete
    }

public:
    // 核心运行逻辑：负责 Socket 的生命周期管理
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

        // IP地址处理
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

        // 3. 绑定 (Bind)
        if (::bind(nListenSocket, (sockaddr *)&ServerAddress, sizeof(sockaddr_in)) == -1) {
            std::cerr << "[Error] bind error: " << strerror(errno) << std::endl;
            ::close(nListenSocket);
            return -1;
        }

        // 4. 监听 (Listen)
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
                continue;
            }

            // 获取客户端IP用于日志
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &ClientAddress.sin_addr, clientIP, INET_ADDRSTRLEN);
            std::cout << "[Server] Client connected: " << clientIP << std::endl;

            // 6. 调用接口方法处理业务
            // 具体执行的是 CMyTCPServer::ServerFunction
            if (m_pObserver != NULL) {
                m_pObserver->ServerFunction(nConnectedSocket, nListenSocket);
            }

            // 业务处理完毕，关闭当前连接
            ::close(nConnectedSocket);
            std::cout << "[Server] Client disconnected: " << clientIP << std::endl;
        }

        ::close(nListenSocket);
        return 0;
    }

private:
    int m_nServerPort;
    std::string m_strBoundIP;
    int m_nLengthOfQueueOfListen;
    CTCPServerObserver *m_pObserver; // 核心：持有接口的指针
};

// -----------------------------------------------------------
// 3. 定义具体的业务类 (Concrete Observer)
// -----------------------------------------------------------
class CMyTCPServer : public CTCPServerObserver {
public:
    CMyTCPServer() {}
    virtual ~CMyTCPServer() {}

private:
    // 实现接口定义的纯虚函数，编写具体的 Echo 逻辑
    virtual void ServerFunction(int nConnectedSocket, int nListenSocket) {
        char buf[MAX_BUFFER_SIZE];

        // 循环接收数据
        while (true) {
            memset(buf, 0, MAX_BUFFER_SIZE);
            ssize_t bytesRead = ::read(nConnectedSocket, buf, MAX_BUFFER_SIZE - 1);

            if (bytesRead > 0) {
                std::cout << "[Recv]: " << buf;
                // 将数据原样回传 (Echo)
                ssize_t bytesWrite = ::write(nConnectedSocket, buf, bytesRead);
                assert(bytesRead == bytesWrite);
            } else if (bytesRead == 0) {
                // 客户端断开连接
                break;
            } else {
                std::cerr << "[Error] Read failed" << std::endl;
                break;
            }
        }
    }
};

int main() {
    // 1. 创建业务对象 (Observer)
    CMyTCPServer myserver;

    // 2. 创建通信对象，并将业务对象传入
    CTCPServer tcpserver(&myserver, DEFAULT_PORT);

    // 3. 运行服务
    tcpserver.Run();

    return 0;
}