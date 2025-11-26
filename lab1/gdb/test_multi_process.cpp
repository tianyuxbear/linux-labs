#include <cstdint> // 用于 intptr_t
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h> // 必须包含，用于waitpid
#include <unistd.h>

using namespace std;

typedef void (*PROCESS)(void *pContext);

pid_t CreateProcess(PROCESS child, void *pContext) {
    pid_t pid = fork();

    if (0 == pid) {
        // 子进程逻辑
        child(pContext);
        exit(0);
    } else if (pid > 0) {
        // 父进程返回子进程ID
        return pid;
    } else {
        cerr << "Fork failed!" << endl;
        return -1;
    }
}

void MyChild(void *pContext) {
    // 使用 intptr_t 安全转换
    long long i = (long long)(intptr_t)pContext;
    cout << "[Child Process] PID: " << getpid() << ", Value: " << i << endl;
}

int main() {
    int i = 90;
    cout << "[Main Process] PID: " << getpid() << " Start." << endl;

    pid_t childPid = CreateProcess(MyChild, (void *)(intptr_t)i);

    if (childPid > 0) {
        cout << "[Main Process] Created child with PID: " << childPid << endl;
        // 等待子进程结束，防止产生僵尸进程
        int status;
        waitpid(childPid, &status, 0);
        cout << "[Main Process] Child finished." << endl;
    }

    return 0;
}