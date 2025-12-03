/*************************************************************************
 * 文件名: client.cpp
 * 编程范式: 基于接口的编程方法
 * 功能: 封装TCP客户端通信库，通过Observer接口分离业务逻辑，实现Echo客户端
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

// -----------------------------------------------------------
// 1. 定义接口类 (Observer Interface) [cite: 366]
// -----------------------------------------------------------
class CTCPClientObserver {
public:
    CTCPClientObserver() {}
    virtual ~CTCPClientObserver() {}

public:
    // 纯虚函数：定义客户端业务逻辑的规范
    virtual void ClientFunction(int nConnectedSocket) = 0;
};

// -----------------------------------------------------------
// 2. 定义通信库类 (Network Wrapper)
// -----------------------------------------------------------
class CTCPClient {
public:
    // 构造函数：接受观察者指针
    CTCPClient(CTCPClientObserver *pObserver, int nServerPort, const char *strServerIP) {
        m_pObserver = pObserver; // 保存接口指针 [cite: 380]
        m_nServerPort = nServerPort;
        if (strServerIP != NULL) {
            m_strServerIP = strServerIP;
        } else {
            m_strServerIP = "127.0.0.1";
        }
    }

    virtual ~CTCPClient() {
        // m_pObserver 生命周期由外部管理
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

        // 4. 调用接口方法执行业务 [cite: 380]
        if (m_pObserver != NULL) {
            m_pObserver->ClientFunction(nClientSocket);
        }

        // 5. 关闭连接
        ::close(nClientSocket);

        return 0;
    }

private:
    int m_nServerPort;
    std::string m_strServerIP;
    CTCPClientObserver *m_pObserver; // 持有接口的指针
};

// -----------------------------------------------------------
// 3. 定义具体的业务类 (Concrete Observer)
// -----------------------------------------------------------
class CMyTCPClient : public CTCPClientObserver {
public:
    CMyTCPClient() {}
    virtual ~CMyTCPClient() {}

private:
    // 实现接口定义的纯虚函数，编写交互逻辑
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

            // 如果输入 quit 则退出
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
    // 1. 创建业务对象 (Observer)
    CMyTCPClient client;

    // 2. 创建通信对象，并将业务对象传入
    CTCPClient tcpclient(&client, SERVER_PORT, SERVER_IP);

    // 3. 运行客户端
    tcpclient.Run();

    return 0;
}