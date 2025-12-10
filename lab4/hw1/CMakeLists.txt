# --------------------------------------------------------
# 1. 编译动态链接库 (插件)
# 对应命令: g++ -fpic -shared -o libfunc.so function.cpp
# --------------------------------------------------------
add_library(func SHARED function.cpp)

# 设置输出名称为 func (CMake 会自动加上前缀 lib 和后缀 .so，生成 libfunc.so)
set_target_properties(func PROPERTIES OUTPUT_NAME "func")

# --------------------------------------------------------
# 2. 编译主程序
# 对应命令: g++ -o main main.cpp -ldl
# --------------------------------------------------------
add_executable(main-hw1 main.cpp)

# 链接 dl 库 (Dynamic Loading library)
# 在 Linux 下必须链接此库才能使用 dlopen 等函数
target_link_libraries(main-hw1 ${CMAKE_DL_LIBS})
