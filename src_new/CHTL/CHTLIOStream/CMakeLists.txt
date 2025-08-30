# CMakeLists.txt for CHTL IOStream
cmake_minimum_required(VERSION 3.16)

# 设置项目信息
project(CHTLIOStream VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置源文件
set(CHTL_IOSTREAM_SOURCES
    CHTLIOStream.cpp
)

set(CHTL_IOSTREAM_HEADERS
    CHTLIOStream.h
)

# 创建静态库
add_library(CHTLIOStream STATIC ${CHTL_IOSTREAM_SOURCES} ${CHTL_IOSTREAM_HEADERS})

# 设置包含目录
target_include_directories(CHTLIOStream PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# 链接依赖
target_link_libraries(CHTLIOStream PUBLIC
    CHTLFileSystem
    CHTLLexer
    CHTLNode
    CHTLError
)

# 设置目标属性
set_target_properties(CHTLIOStream PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${CHTL_IOSTREAM_HEADERS}"
)