# CMakeLists.txt for CHTL Context
cmake_minimum_required(VERSION 3.16)

# 设置项目信息
project(CHTLContext VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置源文件
set(CHTL_CONTEXT_SOURCES
    Context.cpp
)

set(CHTL_CONTEXT_HEADERS
    Context.h
)

# 创建静态库
add_library(CHTLContext STATIC ${CHTL_CONTEXT_SOURCES} ${CHTL_CONTEXT_HEADERS})

# 设置包含目录
target_include_directories(CHTLContext PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# 链接依赖
target_link_libraries(CHTLContext PUBLIC
    CHTLLexer
    CHTLState
)

# 设置目标属性
set_target_properties(CHTLContext PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${CHTL_CONTEXT_HEADERS}"
)