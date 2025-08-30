# CMakeLists.txt for CHTL State
cmake_minimum_required(VERSION 3.16)

# 设置项目信息
project(CHTLState VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置源文件
set(CHTL_STATE_SOURCES
    State.cpp
)

set(CHTL_STATE_HEADERS
    State.h
)

# 创建静态库
add_library(CHTLState STATIC ${CHTL_STATE_SOURCES} ${CHTL_STATE_HEADERS})

# 设置包含目录
target_include_directories(CHTLState PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# 设置目标属性
set_target_properties(CHTLState PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${CHTL_STATE_HEADERS}"
)