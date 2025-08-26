cmake_minimum_required(VERSION 3.14)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加cmake模块路径
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# 设置编译选项
if(MSVC)
    add_compile_options(/utf-8)  # UTF-8支持
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_compile_options(-fexec-charset=UTF-8)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含ANTLR4
set(ANTLR4_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/ANTLR4)
find_package(antlr4-runtime REQUIRED PATHS ${ANTLR4_ROOT})

# 添加子目录
add_subdirectory(src)

# 创建可执行文件
add_executable(chtl src/Main.cpp)

# 链接所有模块
target_link_libraries(chtl PRIVATE
    CHTLScanner
    CHTLDispatcher
    CHTLCompiler
    CHTLJSCompiler
    CSSCompiler
    JSCompiler
    antlr4-runtime
)

# 设置可执行文件属性
set_target_properties(chtl PROPERTIES
    OUTPUT_NAME "chtl"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 安装规则
install(TARGETS chtl
    RUNTIME DESTINATION bin
)

# 创建module目录
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/module/CMOD)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/module/CJMOD)