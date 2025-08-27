cmake_minimum_required(VERSION 3.16)
project(CHTL_Compiler VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(src)

# 收集源文件
file(GLOB_RECURSE CORE_SOURCES "src/Core/*.cpp")
file(GLOB_RECURSE CHTL_SOURCES "src/CHTL/*.cpp")
file(GLOB_RECURSE SCANNER_SOURCES "src/Scanner/*.cpp")
file(GLOB_RECURSE UTIL_SOURCES "src/Util/*.cpp")

# 创建CHTL编译器库
add_library(chtl_compiler_lib STATIC
    ${CORE_SOURCES}
    ${CHTL_SOURCES}
    ${SCANNER_SOURCES}
    ${UTIL_SOURCES}
)

# 创建可执行文件 (暂时只是基础测试)
add_executable(chtl_compiler
    tests/basic_test.cpp
)

# 链接库
target_link_libraries(chtl_compiler chtl_compiler_lib)

# 设置编译器特性
target_compile_features(chtl_compiler_lib PUBLIC cxx_std_17)
target_compile_features(chtl_compiler PUBLIC cxx_std_17)

# 安装目标
install(TARGETS chtl_compiler RUNTIME DESTINATION bin)
install(TARGETS chtl_compiler_lib ARCHIVE DESTINATION lib)

# 创建版本头文件
configure_file(
    "${CMAKE_SOURCE_DIR}/version.h.in"
    "${CMAKE_BINARY_DIR}/version.h"
)

# 输出配置信息
message(STATUS "CHTL Compiler Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Source Directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "  Binary Directory: ${CMAKE_BINARY_DIR}")
message(STATUS "🎯 严格按照CHTL语法文档实现，追求完整与绝对准确！")
message(STATUS "🏗️ 全新架构，从头构建，避免历史遗留问题")
message(STATUS "⚡ 模块化设计，每个组件都有完整实现")