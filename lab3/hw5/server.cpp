/*************************************************************************
 * 文件名: server.cpp
 * 编程范式: 基于方面的编程方法 - AOP / Mixin
 * 功能: 封装TCP服务端，分离连接逻辑(Aspect)与业务逻辑(Core)，实现Echo服务
 *************************************************************************/
#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <string>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024
#define DEFAULT_PORT 5000

// -----------------------------------------------------------
// AOP 切面类：CTCPServer
// 职责：负责次逻辑（网络连接管理、Socket生命周期）
// 技巧：通过继承模板参数 ConnectionProcessor，将网络功能“混入”到业务类中
// -----------------------------------------------------------
template <typename ConnectionProcessor>
class CTCPServer : public ConnectionProcessor {
public:
    // 构造函数：初始化网络参数
    CTCPServer(int nServerPort, int nLengthOfQueueOfListen = 100, const char *strBoundIP = NULL) {
        m_nServerPort = nServerPort;
        m_nLengthOfQueueOfListen = nLengthOfQueueOfListen;

        // 使用 std::string 自动管理内存，替代原有的 new char[]
        if (NULL == strBoundIP) {
            m_strBoundIP = ""; // 空字符串表示 INADDR_ANY
        } else {
            m_strBoundIP = strBoundIP;
        }
    }

    virtual ~CTCPServer() {
        // 资源自动释放
    }

public:
    // 运行连接逻辑（Aspect Logic）
    int Run() {
        // 忽略 SIGPIPE 信号，防止客户端异常断开导致服务端退出
        signal(SIGPIPE, SIG_IGN);

        // 1. 创建 Socket
        int nListenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == nListenSocket) {
            perror("socket");
            return -1;
        }

        // 允许地址重用，方便快速重启
        int on = 1;
        setsockopt(nListenSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

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
            perror("bind");
            ::close(nListenSocket);
            return -1;
        }

        // 3. 监听
        if (::listen(nListenSocket, m_nLengthOfQueueOfListen) == -1) {
            perror("listen");
            ::close(nListenSocket);
            return -1;
        }

        std::cout << "[Server] Listening on port " << m_nServerPort << " ..." << std::endl;

        // 4. 循环处理连接
        while (true) {
            sockaddr_in ClientAddress;
            socklen_t LengthOfClientAddress = sizeof(sockaddr_in);

            // 阻塞等待连接
            int nConnectedSocket = ::accept(nListenSocket, (sockaddr *)&ClientAddress, &LengthOfClientAddress);
            if (-1 == nConnectedSocket) {
                perror("accept");
                continue;
            }

            // 打印客户端信息
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &ClientAddress.sin_addr, clientIP, INET_ADDRSTRLEN);
            std::cout << "[Server] Client connected: " << clientIP << std::endl;

            // 5. 织入业务逻辑 (Weaving)
            // 调用父类（业务类）的方法处理具体数据
            // 这里体现了分离：CTCPServer只管连接，CMyTCPServer只管数据
            ConnectionProcessor *pProcessor = static_cast<ConnectionProcessor *>(this);
            pProcessor->ServerFunction(nConnectedSocket, nListenSocket);

            // 关闭连接
            ::close(nConnectedSocket);
            std::cout << "[Server] Client disconnected: " << clientIP << std::endl;
        }

        ::close(nListenSocket);
        return 0;
    }

private:
    int m_nServerPort;
    std::string m_strBoundIP; // 优化：使用string管理
    int m_nLengthOfQueueOfListen;
};

// -----------------------------------------------------------
// 核心业务类：CMyTCPServer
// 职责：负责主逻辑（业务数据的处理），不关心Socket如何建立
// -----------------------------------------------------------
class CMyTCPServer {
public:
    // 具体的 Echo 业务实现
    void ServerFunction(int nConnectedSocket, int /*nListenSocket*/) {
        char buf[MAX_BUFFER_SIZE];

        while (true) {
            memset(buf, 0, MAX_BUFFER_SIZE);
            // 读取数据
            ssize_t bytesRead = ::read(nConnectedSocket, buf, MAX_BUFFER_SIZE - 1);

            if (bytesRead > 0) {
                std::cout << "[Recv]: " << buf;
                // Echo 回发
                ssize_t bytesWrite = ::write(nConnectedSocket, buf, bytesRead);
            } else if (bytesRead == 0) {
                // 对端关闭
                break;
            } else {
                perror("read");
                break;
            }
        }
    }
};

int main() {
    // AOP 组合：将业务逻辑(CMyTCPServer)织入到网络框架(CTCPServer)中
    CTCPServer<CMyTCPServer> myserver(DEFAULT_PORT);
    myserver.Run();
    return 0;
}