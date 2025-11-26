#include <cstdint> // 用于 intptr_t
#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

void *thread_func(void *arg) {
    long long i = (long long)(intptr_t)arg;
    cout << "  [New Thread] TID: " << pthread_self() << endl;
    cout << "  [New Thread] Arg is: " << i << endl;
    return (void *)0;
}

int main() {
    pthread_t tid;
    cout << "[Main Thread] Start." << endl;

    // 传递参数 2
    if (pthread_create(&tid, NULL, thread_func, (void *)(intptr_t)2) != 0) {
        cerr << "pthread_create error" << endl;
        return 1;
    }

    // 等待线程结束，替代原来的 sleep(500)
    pthread_join(tid, NULL);

    cout << "[Main Thread] Child thread joined. Main TID: " << pthread_self() << endl;
    return 0;
}