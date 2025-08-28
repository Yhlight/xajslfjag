cmake_minimum_required(VERSION 3.10)

# ZipUtil模块
set(ZIPUTIL_SOURCES
    ../ZipUtil.cpp
)

# 创建静态库
add_library(ZipUtil STATIC ${ZIPUTIL_SOURCES})

# 设置包含目录
target_include_directories(ZipUtil PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 设置C++标准
target_compile_features(ZipUtil PUBLIC cxx_std_17)

# 设置编译选项
target_compile_options(ZipUtil PRIVATE -Wall -Wextra)