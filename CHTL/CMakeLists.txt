cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR})

# CHTL 核心库
add_library(CHTLCore STATIC
    # Lexer
    CHTL/CHTLLexer/Token.cpp
    CHTL/CHTLLexer/GlobalMap.cpp
    CHTL/CHTLLexer/Lexer.cpp
    
    # Nodes
    CHTL/CHTLNode/BaseNode.cpp
    CHTL/CHTLNode/CommentNode.cpp
    CHTL/CHTLNode/TemplateNode.cpp
    CHTL/CHTLNode/CustomNode.cpp
    CHTL/CHTLNode/StyleNode.cpp
    CHTL/CHTLNode/ScriptNode.cpp
    CHTL/CHTLNode/OriginNode.cpp
    CHTL/CHTLNode/ImportNode.cpp
    CHTL/CHTLNode/ConfigNode.cpp
    CHTL/CHTLNode/NamespaceNode.cpp
    CHTL/CHTLNode/OperatorNode.cpp
    
    # State
    CHTL/CHTLState/CHTLState.cpp
    
    # Context
    CHTL/CHTLContext/CHTLContext.cpp
)

# CHTL JS 库
add_library(CHTLJSCore STATIC
    # Lexer
    CHTLJS/CHTLJSLexer/Token.cpp
    CHTLJS/CHTLJSLexer/GlobalMap.cpp
    CHTLJS/CHTLJSLexer/Lexer.cpp
    
    # Nodes
    CHTLJS/CHTLJSNode/BaseNode.cpp
    
    # State
    CHTLJS/CHTLJSState/CHTLJSState.cpp
    
    # Context
    CHTLJS/CHTLJSContext/CHTLJSContext.cpp
)

# 工具库
add_library(CHTLUtil STATIC
    Util/FileSystem/FileSystem.cpp
)

# 链接文件系统库 (Clang 20.1 默认支持C++17 filesystem，不需要额外链接)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(CHTLUtil PRIVATE stdc++fs)
endif()

# 主编译器可执行文件
add_executable(chtlc
    main.cpp  # 需要创建
)

target_link_libraries(chtlc
    PRIVATE
        CHTLCore
        CHTLJSCore
        CHTLUtil
)

# 安装规则
install(TARGETS chtlc
    RUNTIME DESTINATION bin
)

# 安装模块目录
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Module/
    DESTINATION module
    FILES_MATCHING
    PATTERN "*.chtl"
    PATTERN "*.cmod"
    PATTERN "*.cjmod"
)

# 测试
enable_testing()
add_subdirectory(Test)