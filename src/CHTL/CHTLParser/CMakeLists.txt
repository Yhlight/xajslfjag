# CMakeLists.txt for CHTL Parser
cmake_minimum_required(VERSION 3.16)

# 设置项目信息
project(CHTLParser VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置源文件
set(CHTL_PARSER_SOURCES
    CHTLParser.cpp
)

set(CHTL_PARSER_HEADERS
    CHTLParser.h
)

# 创建静态库
add_library(CHTLParser STATIC ${CHTL_PARSER_SOURCES} ${CHTL_PARSER_HEADERS})

# 设置包含目录
target_include_directories(CHTLParser PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# 链接依赖
target_link_libraries(CHTLParser PUBLIC
    CHTLLexer
    CHTLState
    CHTLContext
    CHTLNode
    CHTLIOStream
    CHTLError
)

# 设置目标属性
set_target_properties(CHTLParser PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${CHTL_PARSER_HEADERS}"
)