# 简化的CHTL项目构建配置
cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
if(MSVC)
    add_compile_options(/utf-8)  # 支持UTF-8
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# CHTL编译器核心源文件
set(CHTL_CORE_SOURCES
    CHTL/CHTLLexer/CHTLToken.cpp
    CHTL/CHTLLexer/CHTLGlobalMap.cpp
    CHTL/CHTLLexer/CHTLLexer.cpp
    CHTL/CHTLNode/CHTLBaseNode.cpp
    CHTL/CHTLNode/CHTLElementNode.cpp
    CHTL/CHTLNode/CHTLTextNode.cpp
    CHTL/CHTLNode/CHTLCommentNode.cpp
)

# CHTL JS编译器核心源文件
set(CHTLJS_CORE_SOURCES
    CHTLJS/CHTLJSLexer/CHTLJSToken.cpp
    CHTLJS/CHTLJSLexer/CHTLJSGlobalMap.cpp
    CHTLJS/CHTLJSLexer/CHTLJSLexer.cpp
)

# 创建CHTL静态库
add_library(chtl_core STATIC ${CHTL_CORE_SOURCES})
target_include_directories(chtl_core PUBLIC .)

# 创建CHTL JS静态库
add_library(chtljs_core STATIC ${CHTLJS_CORE_SOURCES})
target_include_directories(chtljs_core PUBLIC .)

# 创建测试可执行文件
add_executable(chtl_test
    Test/test_main.cpp
)

target_link_libraries(chtl_test 
    chtl_core 
    chtljs_core
)

# 创建简单演示程序
add_executable(chtl_demo
    main.cpp
)

target_link_libraries(chtl_demo 
    chtl_core 
    chtljs_core
)