# 编译器设置
CXX = g++
# 编译选项：-g 用于生成调试信息(GDB必须)，-O0 禁止优化以便调试，-pthread 用于线程库
CXXFLAGS = -g -O0 -pthread -Wall

# 目标文件
TARGETS = test_multi_process test_multi_thread

# 默认目标
all: $(TARGETS)

# 编译多进程程序
test_multi_process: test_multi_process.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译多线程程序
test_multi_thread: test_multi_thread.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

# 清理编译文件
clean:
	rm -f $(TARGETS)