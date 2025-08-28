cmake_minimum_required(VERSION 3.10)

# FileStream模块
set(FILESTREAM_SOURCES
    FileStream.cpp
)

# 创建静态库
add_library(FileStream STATIC ${FILESTREAM_SOURCES})

# 设置包含目录
target_include_directories(FileStream PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 设置C++标准
target_compile_features(FileStream PUBLIC cxx_std_17)

# 设置编译选项
target_compile_options(FileStream PRIVATE -Wall -Wextra)