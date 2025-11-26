# 编译器设置
CXX = g++
# -g: 生成调试信息 (Valgrind需要它来定位行号)
# -O0: 禁止优化 (防止编译器优化掉错误逻辑)
CXXFLAGS = -g -O0 -Wall

# 目标程序名
TARGET = memory_test

all: $(TARGET)

$(TARGET): memory_test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)