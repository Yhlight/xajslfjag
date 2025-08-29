# CMakeLists.txt for CHTL FileSystem
cmake_minimum_required(VERSION 3.16)

# 设置项目信息
project(CHTLFileSystem VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置源文件
set(CHTL_FILESYSTEM_SOURCES
    FileSystem.cpp
)

set(CHTL_FILESYSTEM_HEADERS
    FileSystem.h
)

# 创建静态库
add_library(CHTLFileSystem STATIC ${CHTL_FILESYSTEM_SOURCES} ${CHTL_FILESYSTEM_HEADERS})

# 设置包含目录
target_include_directories(CHTLFileSystem PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# 链接系统库
if(WIN32)
    # Windows特定库
    target_link_libraries(CHTLFileSystem PRIVATE)
else()
    # Unix/Linux特定库
    target_link_libraries(CHTLFileSystem PRIVATE pthread)
endif()

# 设置目标属性
set_target_properties(CHTLFileSystem PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${CHTL_FILESYSTEM_HEADERS}"
)