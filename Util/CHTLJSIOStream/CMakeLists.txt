cmake_minimum_required(VERSION 3.10)

# CHTLJSIOStream模块
set(CHTLJSIOSTREAM_SOURCES
    CHTLJSIOStream.cpp
)

# 创建静态库
add_library(CHTLJSIOStream STATIC ${CHTLJSIOSTREAM_SOURCES})

# 设置包含目录
target_include_directories(CHTLJSIOStream PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 设置C++标准
target_compile_features(CHTLJSIOStream PUBLIC cxx_std_17)

# 设置编译选项
target_compile_options(CHTLJSIOStream PRIVATE -Wall -Wextra)