/*************************************************************************
 * 文件名: client.cpp
 * 编程范式: 传统C语言的结构化编程方法
 * 功能: 封装TCP客户端通信库，并实现Echo客户端
 *************************************************************************/
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_PORT 5000
#define SERVER_IP "127.0.0.1"

// 定义回调函数指针类型
typedef void (*TCPClientCallback)(int nConnectedSocket);

/**
 * @brief 封装TCP客户端初始化及连接逻辑
 * @param ClientFunction 用户自定义的业务逻辑回调函数
 * @param nServerPort 服务器端口
 * @param strServerIP 服务器IP地址
 * @return int 成功返回0，失败返回-1
 */
int RunTCPClient(TCPClientCallback ClientFunction, int nServerPort, const char *strServerIP) {
    // 1. 创建套接字
    int nClientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == nClientSocket) {
        std::cerr << "[Error] Create socket failed." << std::endl;
        return -1;
    }

    // 2. 初始化服务器地址结构
    sockaddr_in ServerAddress;
    memset(&ServerAddress, 0, sizeof(sockaddr_in));
    ServerAddress.sin_family = AF_INET;

    // IP地址转换
    if (::inet_pton(AF_INET, strServerIP, &ServerAddress.sin_addr) != 1) {
        std::cerr << "[Error] inet_pton error (Invalid IP)." << std::endl;
        ::close(nClientSocket);
        return -1;
    }
    // 端口转换
    ServerAddress.sin_port = htons(nServerPort);

    // 3. 连接服务器 (Connect)
    std::cout << "[Client] Connecting to " << strServerIP << ":" << nServerPort << "..." << std::endl;
    if (::connect(nClientSocket, (sockaddr *)&ServerAddress, sizeof(ServerAddress)) == -1) {
        std::cerr << "[Error] Connect failed." << std::endl;
        ::close(nClientSocket);
        return -1;
    }

    std::cout << "[Client] Connected successfully!" << std::endl;

    // 4. 执行用户逻辑
    ClientFunction(nClientSocket);

    // 5. 关闭连接
    ::close(nClientSocket);

    return 0;
}

/**
 * @brief 用户自定义的业务逻辑：发送输入并接收回显
 */
void MyEchoClient(int nConnectedSocket) {
    char sendBuf[MAX_BUFFER_SIZE];
    char recvBuf[MAX_BUFFER_SIZE];

    std::cout << "Please input message (type 'quit' to exit):" << std::endl;

    while (true) {
        std::cout << "Input: ";
        // 从标准输入读取一行
        if (fgets(sendBuf, MAX_BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        // 如果输入 quit 则退出
        if (strncmp(sendBuf, "quit", 4) == 0) {
            break;
        }

        // 发送数据到服务器
        ssize_t writeLen = ::write(nConnectedSocket, sendBuf, strlen(sendBuf));
        if (writeLen < 0) {
            std::cerr << "[Error] Write failed." << std::endl;
            break;
        }

        // 接收服务器的回显
        memset(recvBuf, 0, MAX_BUFFER_SIZE);
        ssize_t readLen = ::read(nConnectedSocket, recvBuf, MAX_BUFFER_SIZE - 1);

        if (readLen > 0) {
            std::cout << "Echo form Server: " << recvBuf << std::endl;
        } else if (readLen == 0) {
            std::cout << "[Client] Server closed connection." << std::endl;
            break;
        } else {
            std::cerr << "[Error] Read failed." << std::endl;
            break;
        }
    }
}

int main() {
    // 启动客户端，连接到本地 5000 端口
    RunTCPClient(MyEchoClient, SERVER_PORT, SERVER_IP);
    return 0;
}