# CMakeLists.txt for CHTL Generator
cmake_minimum_required(VERSION 3.16)

# 设置项目信息
project(CHTLGenerator VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置源文件
set(CHTL_GENERATOR_SOURCES
    # 将来添加具体的实现文件
)

set(CHTL_GENERATOR_HEADERS
    CHTLGenerator.h
)

# 创建静态库
add_library(CHTLGenerator STATIC ${CHTL_GENERATOR_HEADERS})

# 设置为仅头文件库
set_target_properties(CHTLGenerator PROPERTIES LINKER_LANGUAGE CXX)

# 设置包含目录
target_include_directories(CHTLGenerator PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# 链接依赖
target_link_libraries(CHTLGenerator PUBLIC
    CHTLNode
    CHTLIOStream
    CHTLError
)

# 设置目标属性
set_target_properties(CHTLGenerator PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${CHTL_GENERATOR_HEADERS}"
)