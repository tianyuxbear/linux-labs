/*************************************************************************
 * 文件名: server.cpp
 * 编程范式: 传统C语言的结构化编程方法
 * 功能: 封装TCP服务端通信库，并实现Echo服务器
 *************************************************************************/
#include <arpa/inet.h>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <memory.h>
#include <netinet/in.h>
#include <unistd.h>

// 定义最大缓冲区大小
#define MAX_BUFFER_SIZE 1024
// 定义默认端口
#define DEFAULT_PORT 5000

// 定义回调函数指针类型，用于处理具体的业务逻辑
// nConnectedSocket: 已连接的套接字描述符
// clientIP: 客户端IP地址字符串
typedef void (*TCPServerCallback)(int nConnectedSocket, const char *clientIP);

/**
 * @brief 封装TCP服务端初始化及运行逻辑
 * @param ServerFunction 用户自定义的业务逻辑回调函数
 * @param nPort 监听端口号
 * @param nLengthOfQueueOfListen 监听队列最大长度，默认100
 * @param strBoundIP 绑定的IP地址，NULL表示绑定所有本地IP (INADDR_ANY)
 * @return int 成功返回0，失败返回-1
 */
int RunTCPServer(TCPServerCallback ServerFunction, int nPort, int nLengthOfQueueOfListen = 100, const char *strBoundIP = NULL) {
    // 1. 创建套接字 (Socket)
    // AF_INET: IPv4协议, SOCK_STREAM: TCP流式传输
    int nListenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == nListenSocket) {
        std::cerr << "[Error] Create socket failed: " << strerror(errno) << std::endl;
        return -1;
    }

    // 2. 初始化服务器地址结构体
    sockaddr_in ServerAddress;
    memset(&ServerAddress, 0, sizeof(sockaddr_in)); // 清零
    ServerAddress.sin_family = AF_INET;

    // 处理IP地址绑定
    if (NULL == strBoundIP) {
        // 绑定本机所有IP
        ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        // 绑定指定IP，将点分十进制转换为网络字节序
        if (::inet_pton(AF_INET, strBoundIP, &ServerAddress.sin_addr) != 1) {
            std::cerr << "[Error] inet_pton failed." << std::endl;
            ::close(nListenSocket);
            return -1;
        }
    }
    // 绑定端口，主机字节序转网络字节序
    ServerAddress.sin_port = htons(nPort);

    // 3. 绑定套接字与地址 (Bind)
    if (::bind(nListenSocket, (sockaddr *)&ServerAddress, sizeof(sockaddr_in)) == -1) {
        std::cerr << "[Error] Bind failed: " << strerror(errno) << std::endl;
        ::close(nListenSocket);
        return -1;
    }

    // 4. 进入监听状态 (Listen)
    if (::listen(nListenSocket, nLengthOfQueueOfListen) == -1) {
        std::cerr << "[Error] Listen failed: " << strerror(errno) << std::endl;
        ::close(nListenSocket);
        return -1;
    }

    std::cout << "[Server] Listening on port " << nPort << "..." << std::endl;

    // 5. 循环接受客户端连接 (Accept Loop)
    while (true) {
        sockaddr_in ClientAddress;
        socklen_t LengthOfClientAddress = sizeof(sockaddr_in);

        // 阻塞等待客户端连接
        int nConnectedSocket = ::accept(nListenSocket, (sockaddr *)&ClientAddress, &LengthOfClientAddress);
        if (-1 == nConnectedSocket) {
            std::cerr << "[Error] Accept failed." << std::endl;
            // accept失败通常不应导致服务器退出，而是继续尝试
            continue;
        }

        // 获取客户端IP地址便于日志打印
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ClientAddress.sin_addr, clientIP, INET_ADDRSTRLEN);

        // 6. 调用回调函数处理业务逻辑
        ServerFunction(nConnectedSocket, clientIP);

        // 7. 处理完毕，关闭已连接套接字
        ::close(nConnectedSocket);
    }

    // 关闭监听套接字 (实际上在这个无限循环中很难执行到这里)
    ::close(nListenSocket);
    return 0;
}

/**
 * @brief 用户自定义的业务逻辑：Echo服务
 * 接收客户端发送的数据，打印并原样发回
 */
void MyEchoServer(int nConnectedSocket, const char *clientIP) {
    std::cout << "[Server] Client connected from: " << clientIP << std::endl;

    char buf[MAX_BUFFER_SIZE];

    while (true) {
        memset(buf, 0, MAX_BUFFER_SIZE);
        // 读取数据
        ssize_t bytesRead = ::read(nConnectedSocket, buf, MAX_BUFFER_SIZE - 1);

        if (bytesRead > 0) {
            std::cout << "[Recv from " << clientIP << "]: " << buf;
            // Echo回写数据
            ssize_t bytesWrite = ::write(nConnectedSocket, buf, bytesRead);
            assert(bytesRead == bytesWrite);
        } else if (bytesRead == 0) {
            // read返回0表示对端关闭连接
            std::cout << "[Server] Client disconnected: " << clientIP << std::endl;
            break;
        } else {
            std::cerr << "[Error] Read error." << std::endl;
            break;
        }
    }
}

int main() {
    // 启动服务器，使用端口5000
    // 删除了原代码中导致死循环的 while(true) {}
    RunTCPServer(MyEchoServer, DEFAULT_PORT);
    return 0;
}