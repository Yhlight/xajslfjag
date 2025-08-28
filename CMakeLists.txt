cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置UTF-8编码支持
if(MSVC)
    add_compile_options(/utf-8)
else()
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找ANTLR4
find_package(ANTLR4 REQUIRED)

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/ANTLR4/include
    ${CMAKE_SOURCE_DIR}/css
    ${CMAKE_SOURCE_DIR}/js
)

# 链接目录
link_directories(
    ${CMAKE_SOURCE_DIR}/ANTLR4/lib
)

# 子目录
add_subdirectory(src)

# 主程序
add_executable(chtl_compiler
    src/main.cpp
)

# 链接库
target_link_libraries(chtl_compiler
    chtl_core
    chtljs_core
    css_compiler
    js_compiler
    scanner
    dispatcher
    util
    module_system
    ${ANTLR4_LIBRARIES}
)

# 安装规则
install(TARGETS chtl_compiler
    RUNTIME DESTINATION bin
)

# 安装模块目录
install(DIRECTORY src/Module/
    DESTINATION share/chtl/modules
    FILES_MATCHING PATTERN "*.chtl" PATTERN "*.cmod" PATTERN "*.cjmod"
)