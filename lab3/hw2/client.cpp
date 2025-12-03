/*************************************************************************
 * 文件名: client.cpp
 * 编程范式: 面向对象的编程方法 (3.14)
 * 功能: 封装TCP客户端类，通过继承和多态实现Echo客户端
 *************************************************************************/
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_PORT 5000
#define SERVER_IP "127.0.0.1"

// 基类：CTCPClient
// 负责底层的Socket创建和连接
class CTCPClient {
public:
    CTCPClient(int nServerPort, const char *strServerIP) {
        m_nServerPort = nServerPort;
        // 使用 std::string 管理内存
        if (strServerIP != NULL) {
            m_strServerIP = strServerIP;
        } else {
            m_strServerIP = "127.0.0.1";
        }
    }

    virtual ~CTCPClient() {
    }

public:
    // 核心运行逻辑
    int Run() {
        // 1. 创建套接字
        int nClientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == nClientSocket) {
            std::cerr << "[Error] socket error" << std::endl;
            return -1;
        }

        // 2. 初始化服务器地址
        sockaddr_in ServerAddress;
        memset(&ServerAddress, 0, sizeof(sockaddr_in));
        ServerAddress.sin_family = AF_INET;

        if (::inet_pton(AF_INET, m_strServerIP.c_str(), &ServerAddress.sin_addr) != 1) {
            std::cerr << "[Error] inet_pton error" << std::endl;
            ::close(nClientSocket);
            return -1;
        }

        ServerAddress.sin_port = htons(m_nServerPort);

        // 3. 连接服务器 (Connect)
        std::cout << "[Client] Connecting to " << m_strServerIP << ":" << m_nServerPort << "..." << std::endl;
        if (::connect(nClientSocket, (sockaddr *)&ServerAddress, sizeof(ServerAddress)) == -1) {
            std::cerr << "[Error] connect error" << std::endl;
            ::close(nClientSocket);
            return -1;
        }

        std::cout << "[Client] Connected!" << std::endl;

        // 4. 多态调用：调用派生类实现的具体业务逻辑
        ClientFunction(nClientSocket);

        // 5. 关闭连接
        ::close(nClientSocket);

        return 0;
    }

    // 虚函数：由派生类实现
    virtual void ClientFunction(int nConnectedSocket) {
    }

private:
    int m_nServerPort;
    std::string m_strServerIP; // 使用 string 替代 char*
};

// 派生类：CMyTCPClient
// 继承自 CTCPClient，重写 ClientFunction 实现具体的交互逻辑
class CMyTCPClient : public CTCPClient {
public:
    CMyTCPClient(int nServerPort, const char *strServerIP)
        : CTCPClient(nServerPort, strServerIP) {
    }

    virtual ~CMyTCPClient() {
    }

private:
    // 重写父类的虚函数，实现用户输入发送和接收回显
    virtual void ClientFunction(int nConnectedSocket) {
        char sendBuf[MAX_BUFFER_SIZE];
        char recvBuf[MAX_BUFFER_SIZE];

        std::cout << "Please input message (type 'quit' to exit):" << std::endl;

        while (true) {
            std::cout << "Input: ";
            // 获取用户输入
            if (fgets(sendBuf, MAX_BUFFER_SIZE, stdin) == NULL) {
                break;
            }

            // 输入 quit 退出
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
            } else if (readLen == 0) {
                std::cout << "[Client] Server closed connection" << std::endl;
                break;
            } else {
                std::cerr << "[Error] Read failed" << std::endl;
                break;
            }
        }
    }
};

int main() {
    // 实例化派生类对象并运行
    CMyTCPClient client(SERVER_PORT, SERVER_IP);
    client.Run();

    return 0;
}