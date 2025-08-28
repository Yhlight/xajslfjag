cmake_minimum_required(VERSION 3.10)

# CHTLIOStream模块
set(CHTLIOSTREAM_SOURCES
    CHTLIOStream.cpp
)

# 创建静态库
add_library(CHTLIOStream STATIC ${CHTLIOSTREAM_SOURCES})

# 设置包含目录
target_include_directories(CHTLIOStream PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 设置C++标准
target_compile_features(CHTLIOStream PUBLIC cxx_std_17)

# 设置编译选项
target_compile_options(CHTLIOStream PRIVATE -Wall -Wextra)