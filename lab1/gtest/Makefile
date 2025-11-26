# 1. 编译器设置
CXX = g++
# 确保使用 c++17
CXXFLAGS = -g -Wall -std=c++17

# 2. 关键路径设置：优先查找 /usr/local (手动安装的位置在这里)
# -I 告诉编译器去哪找 .h 头文件
INCLUDES = -I/usr/local/include
# -L 告诉链接器去哪找 .a/.so 库文件
LDFLAGS = -L/usr/local/lib

# 3. 链接库
LDLIBS = -lgtest -lpthread

# 4. 目标定义
TARGET = run_tests
SRCS = main.cpp CAdder.cpp CAdderTester.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

# 链接步骤：加上 $(LDFLAGS) 和 $(LDLIBS)
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# 编译步骤：加上 $(INCLUDES)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)