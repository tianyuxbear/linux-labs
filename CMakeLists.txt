cmake_minimum_required(VERSION 3.20)

# Project configuration
project(linux-course LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set encoding to UTF-8 (CMake 3.20+)
set(CMAKE_CXX_SOURCE_FILE_ENCODING "UTF-8")

# Enable compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build modes
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()


add_subdirectory(lab2)
add_subdirectory(lab3)