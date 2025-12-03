/*************************************************************************
 * 文件名: client.cpp
 * 编程范式: 静态的面向对象的编程方法 - 模板/CRTP
 * 功能: 使用静态多态封装TCP客户端，实现Echo客户端
 *************************************************************************/
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_PORT 5000
#define SERVER_IP "127.0.0.1"

template <typename T>
class CTCPClient {
public:
    CTCPClient(int nServerPort, const char *strServerIP) {
        m_nServerPort = nServerPort;
        if (strServerIP != NULL) {
            m_strServerIP = strServerIP;
        } else {
            m_strServerIP = "127.0.0.1";
        }
    }

    ~CTCPClient() {
    }

public:
    int Run() {
        int nClientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == nClientSocket) {
            std::cout << "[Error] socket error" << std::endl;
            return -1;
        }

        sockaddr_in ServerAddress;
        memset(&ServerAddress, 0, sizeof(sockaddr_in));
        ServerAddress.sin_family = AF_INET;
        if (::inet_pton(AF_INET, m_strServerIP.c_str(), &ServerAddress.sin_addr) != 1) {
            std::cout << "[Error] inet_pton error" << std::endl;
            ::close(nClientSocket);
            return -1;
        }

        ServerAddress.sin_port = htons(m_nServerPort);

        std::cout << "[Client] Connecting to " << m_strServerIP << ":" << m_nServerPort << "..." << std::endl;
        if (::connect(nClientSocket, (sockaddr *)&ServerAddress, sizeof(ServerAddress)) == -1) {
            std::cout << "[Error] connect error" << std::endl;
            ::close(nClientSocket);
            return -1;
        }

        std::cout << "[Client] Connected!" << std::endl;

        // 静态多态调用
        // 编译期决定调用 T::ClientFunction
        T *pT = static_cast<T *>(this);
        pT->ClientFunction(nClientSocket);

        ::close(nClientSocket);

        return 0;
    }

    void ClientFunction(int nConnectedSocket) {
        // 默认实现
    }

private:
    int m_nServerPort;
    std::string m_strServerIP;
};

// 子类继承自 模板基类<子类>
class CMyTCPClient : public CTCPClient<CMyTCPClient> {
public:
    CMyTCPClient(int nServerPort, const char *strServerIP)
        : CTCPClient<CMyTCPClient>(nServerPort, strServerIP) {
    }

    ~CMyTCPClient() {
    }

    // 实现具体的交互逻辑
    void ClientFunction(int nConnectedSocket) {
        char sendBuf[MAX_BUFFER_SIZE];
        char recvBuf[MAX_BUFFER_SIZE];

        std::cout << "Please input message (type 'quit' to exit):" << std::endl;

        while (true) {
            std::cout << "Input: ";
            if (fgets(sendBuf, MAX_BUFFER_SIZE, stdin) == NULL) {
                break;
            }
            if (strncmp(sendBuf, "quit", 4) == 0) {
                break;
            }

            // 发送
            ssize_t writeLen = ::write(nConnectedSocket, sendBuf, strlen(sendBuf));
            if (writeLen < 0) {
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
    CMyTCPClient client(SERVER_PORT, SERVER_IP);
    client.Run();

    return 0;
}