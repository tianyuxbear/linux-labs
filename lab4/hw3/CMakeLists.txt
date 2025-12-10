# ========================================================
# 关键设置：将所有生成的库文件(.so)自动输出到 build/plugin 目录下
# 符合 PPT 中将插件放入 plugin 子目录的要求
# ========================================================
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/plugin)

# 1. 编译插件 1 (Hello World)
add_library(func1-hw3 SHARED function1.cpp)
set_target_properties(func1-hw3 PROPERTIES OUTPUT_NAME "func1")

# 2. 编译插件 2 (Hello China)
add_library(func2-hw3 SHARED function2.cpp)
set_target_properties(func2-hw3 PROPERTIES OUTPUT_NAME "func2")

# 3. 编译主程序
# 主程序包含 main.cpp 和 CPluginEnumerator.cpp
add_executable(main-hw3 main.cpp CPluginEnumerator.cpp CPluginController.cpp)

# 链接 dl 库
target_link_libraries(main-hw3 ${CMAKE_DL_LIBS})