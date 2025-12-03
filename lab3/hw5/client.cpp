/*************************************************************************
 * 文件名: client.cpp
 * 编程范式: 基于方面的编程方法- AOP / Mixin
 * 功能: 封装TCP客户端，分离连接逻辑与业务逻辑，实现Echo客户端
 *************************************************************************/
#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_PORT 5000
#define SERVER_IP "127.0.0.1"

// -----------------------------------------------------------
// AOP 切面类：CTCPClient
// 职责：负责次逻辑（网络连接建立）
// -----------------------------------------------------------
template <typename ConnectionProcessor>
class CTCPClient : public ConnectionProcessor {
public:
    CTCPClient(int nServerPort, const char *strServerIP) {
        m_nServerPort = nServerPort;

        // 使用 std::string 优化资源管理
        if (strServerIP != NULL) {
            m_strServerIP = strServerIP;
        } else {
            m_strServerIP = "127.0.0.1";
        }
    }

    virtual ~CTCPClient() {
    }

public:
    int Run() {
        // 1. 创建 Socket
        int nClientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == nClientSocket) {
            std::cerr << "[Error] socket error" << std::endl;
            return -1;
        }

        // 2. 准备连接地址
        sockaddr_in ServerAddress;
        memset(&ServerAddress, 0, sizeof(sockaddr_in));
        ServerAddress.sin_family = AF_INET;

        if (::inet_pton(AF_INET, m_strServerIP.c_str(), &ServerAddress.sin_addr) != 1) {
            std::cerr << "[Error] inet_pton error" << std::endl;
            ::close(nClientSocket);
            return -1;
        }

        ServerAddress.sin_port = htons(m_nServerPort);

        // 3. 建立连接
        std::cout << "[Client] Connecting to " << m_strServerIP << ":" << m_nServerPort << "..." << std::endl;
        if (::connect(nClientSocket, (sockaddr *)&ServerAddress, sizeof(ServerAddress)) == -1) {
            std::cerr << "[Error] connect error" << std::endl;
            ::close(nClientSocket);
            return -1;
        }

        std::cout << "[Client] Connected!" << std::endl;

        // 4. 织入业务逻辑
        // 调用父类（业务类）的方法
        ConnectionProcessor *pProcess = static_cast<ConnectionProcessor *>(this);
        pProcess->ClientFunction(nClientSocket);

        // 5. 关闭资源
        ::close(nClientSocket);

        return 0;
    }

private:
    int m_nServerPort;
    std::string m_strServerIP;
};

// -----------------------------------------------------------
// 核心业务类：CMyTCPClient
// 职责：负责主逻辑（用户交互、数据收发）
// -----------------------------------------------------------
class CMyTCPClient {
public:
    CMyTCPClient() {
    }

    virtual ~CMyTCPClient() {
    }

    // 具体的交互业务实现
    void ClientFunction(int nConnectedSocket) {
        char sendBuf[MAX_BUFFER_SIZE];
        char recvBuf[MAX_BUFFER_SIZE];

        std::cout << "Please input message (type 'quit' to exit):" << std::endl;

        while (true) {
            std::cout << "Input: ";
            // 获取用户输入
            if (fgets(sendBuf, MAX_BUFFER_SIZE, stdin) == NULL) {
                break;
            }
            if (strncmp(sendBuf, "quit", 4) == 0) {
                break;
            }

            // 发送数据
            ssize_t writeLen = ::write(nConnectedSocket, sendBuf, strlen(sendBuf));
            if (writeLen < 0) {
                std::cerr << "[Error] Write failed" << std::endl;
                break;
            }

            // 接收回显
            memset(recvBuf, 0, MAX_BUFFER_SIZE);
            ssize_t readLen = ::read(nConnectedSocket, recvBuf, MAX_BUFFER_SIZE - 1);

            if (readLen > 0) {
                std::cout << "Echo from Server: " << recvBuf << std::endl;
            } else {
                std::cout << "[Client] Server disconnected." << std::endl;
                break;
            }
        }
    }
};

int main() {
    // AOP 组合：客户端业务 + 网络连接框架
    // 注意：这里端口修改为 5000 以匹配服务端
    CTCPClient<CMyTCPClient> client(SERVER_PORT, SERVER_IP);
    client.Run();

    return 0;
}