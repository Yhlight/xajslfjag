# CHTL项目 CMake配置文件
# 严格按照目标规划，支持C++17，UTF-8编码，集成ANTLR4
# 模块化开发，CHTL和CHTL JS完全分离

cmake_minimum_required(VERSION 3.16)
project(CHTL_Project 
    VERSION 1.0.0
    DESCRIPTION "CHTL超文本语言编译器项目"
    LANGUAGES CXX
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置UTF-8编码支持
if(MSVC)
    # Visual Studio UTF-8支持
    add_compile_options(/utf-8)
    add_compile_options(/source-charset:utf-8)
    add_compile_options(/execution-charset:utf-8)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DNOMINMAX)
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
endif()

# 设置编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -O2)
    add_compile_options(-finput-charset=UTF-8)
    add_compile_options(-fexec-charset=UTF-8)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/ANTLR4/include)

# ANTLR4库路径
if(WIN32)
    link_directories(${CMAKE_SOURCE_DIR}/ThirdParty/ANTLR4/lib/windows)
else()
    link_directories(${CMAKE_SOURCE_DIR}/ThirdParty/ANTLR4/lib/linux)
endif()

# ============================================================================
# CHTL编译器模块（完全独立）
# ============================================================================

# CHTL词法分析器
add_library(CHTL_Lexer STATIC
    CHTL/CHTLLexer/CHTL词法单元.cpp
    CHTL/CHTLLexer/CHTL词法单元.h
    CHTL/CHTLLexer/CHTL全局映射表.cpp
    CHTL/CHTLLexer/CHTL全局映射表.h
    CHTL/CHTLLexer/CHTL词法分析器.cpp
    CHTL/CHTLLexer/CHTL词法分析器.h
)

# CHTL语法分析器
add_library(CHTL_Parser STATIC
    # 待实现的CHTL解析器文件
)

# CHTL代码生成器
add_library(CHTL_Generator STATIC
    # 待实现的CHTL生成器文件
)

# CHTL状态管理器
add_library(CHTL_State STATIC
    # 待实现的CHTL状态管理器文件
)

# CHTL上下文管理器
add_library(CHTL_Context STATIC
    # 待实现的CHTL上下文管理器文件
)

# CHTL节点系统
add_library(CHTL_Node STATIC
    # 待实现的CHTL节点文件
)

# CHTL模块加载器
add_library(CHTL_Loader STATIC
    # 待实现的CHTL加载器文件
)

# CHTL管理器
add_library(CHTL_Manage STATIC
    # 待实现的CHTL管理器文件
)

# CHTL IO流
add_library(CHTL_IOStream STATIC
    # 待实现的CHTL IO流文件
)

# CMOD系统
add_library(CMOD_System STATIC
    # 待实现的CMOD系统文件
)

# ============================================================================
# CHTL JS编译器模块（完全独立，不与CHTL共享）
# ============================================================================

# CHTL JS词法分析器
add_library(CHTLJS_Lexer STATIC
    CHTLJS/CHTLJSLexer/CHTLJS词法单元.cpp
    CHTLJS/CHTLJSLexer/CHTLJS词法单元.h
    CHTLJS/CHTLJSLexer/CHTLJS全局映射表.cpp
    CHTLJS/CHTLJSLexer/CHTLJS全局映射表.h
    CHTLJS/CHTLJSLexer/CHTLJS词法分析器.cpp
    CHTLJS/CHTLJSLexer/CHTLJS词法分析器.h
)

# CHTL JS语法分析器
add_library(CHTLJS_Parser STATIC
    # 待实现的CHTL JS解析器文件
)

# CHTL JS代码生成器
add_library(CHTLJS_Generator STATIC
    # 待实现的CHTL JS生成器文件
)

# CHTL JS状态管理器
add_library(CHTLJS_State STATIC
    # 待实现的CHTL JS状态管理器文件
)

# CHTL JS上下文管理器
add_library(CHTLJS_Context STATIC
    # 待实现的CHTL JS上下文管理器文件
)

# CHTL JS节点系统
add_library(CHTLJS_Node STATIC
    # 待实现的CHTL JS节点文件
)

# CHTL JS模块加载器
add_library(CHTLJS_Loader STATIC
    # 待实现的CHTL JS加载器文件
)

# CHTL JS管理器
add_library(CHTLJS_Manage STATIC
    # 待实现的CHTL JS管理器文件
)

# CHTL JS IO流
add_library(CHTLJS_IOStream STATIC
    # 待实现的CHTL JS IO流文件
)

# CJMOD系统
add_library(CJMOD_System STATIC
    # 待实现的CJMOD系统文件
)

# ============================================================================
# CSS编译器模块（使用现有ANTLR4生成的代码）
# ============================================================================

add_library(CSS_Compiler STATIC
    CSS/css3Lexer.cpp
    CSS/css3Lexer.h
    CSS/css3Parser.cpp
    CSS/css3Parser.h
    CSS/css3ParserBaseListener.cpp
    CSS/css3ParserBaseListener.h
    CSS/css3ParserListener.cpp
    CSS/css3ParserListener.h
)

target_link_libraries(CSS_Compiler antlr4-runtime)

# ============================================================================
# JS编译器模块（使用现有ANTLR4生成的代码）
# ============================================================================

add_library(JS_Compiler STATIC
    JS/JavaScriptLexer.cpp
    JS/JavaScriptLexer.h
    JS/JavaScriptParser.cpp
    JS/JavaScriptParser.h
    JS/JavaScriptLexerBase.cpp
    JS/JavaScriptLexerBase.h
    JS/JavaScriptParserBase.cpp
    JS/JavaScriptParserBase.h
    JS/JavaScriptParserBaseListener.cpp
    JS/JavaScriptParserBaseListener.h
    JS/JavaScriptParserListener.cpp
    JS/JavaScriptParserListener.h
)

target_link_libraries(JS_Compiler antlr4-runtime)

# ============================================================================
# 统一扫描器模块
# ============================================================================

add_library(Unified_Scanner STATIC
    # 待实现的统一扫描器文件
)

# ============================================================================
# 编译器调度器模块
# ============================================================================

add_library(Compiler_Dispatcher STATIC
    # 待实现的编译器调度器文件
)

# ============================================================================
# 工具模块
# ============================================================================

# 文件系统工具
add_library(File_System STATIC
    # 待实现的文件系统工具文件
)

# ZIP工具
add_library(ZIP_Util STATIC
    # 待实现的ZIP工具文件
)

# ============================================================================
# 主程序
# ============================================================================

add_executable(chtl_compiler
    # 待实现的主程序文件
)

# 链接所有库
target_link_libraries(chtl_compiler
    CHTL_Lexer
    CHTL_Parser
    CHTL_Generator
    CHTL_State
    CHTL_Context
    CHTL_Node
    CHTL_Loader
    CHTL_Manage
    CHTL_IOStream
    CMOD_System
    
    CHTLJS_Lexer
    CHTLJS_Parser
    CHTLJS_Generator
    CHTLJS_State
    CHTLJS_Context
    CHTLJS_Node
    CHTLJS_Loader
    CHTLJS_Manage
    CHTLJS_IOStream
    CJMOD_System
    
    CSS_Compiler
    JS_Compiler
    Unified_Scanner
    Compiler_Dispatcher
    File_System
    ZIP_Util
    
    antlr4-runtime
)

# ============================================================================
# 测试程序
# ============================================================================

# 启用测试
enable_testing()

# CHTL测试
add_executable(test_chtl
    # 待实现的CHTL测试文件
)

target_link_libraries(test_chtl
    CHTL_Lexer
    CHTL_Parser
    CHTL_Generator
)

add_test(NAME CHTL_Test COMMAND test_chtl)

# CHTL JS测试
add_executable(test_chtljs
    # 待实现的CHTL JS测试文件
)

target_link_libraries(test_chtljs
    CHTLJS_Lexer
    CHTLJS_Parser
    CHTLJS_Generator
)

add_test(NAME CHTLJS_Test COMMAND test_chtljs)

# ============================================================================
# 安装配置
# ============================================================================

install(TARGETS chtl_compiler
    RUNTIME DESTINATION bin
)

install(DIRECTORY Module/
    DESTINATION share/chtl/modules
    FILES_MATCHING PATTERN "*.cmod" PATTERN "*.cjmod" PATTERN "*.chtl"
)

# 设置包信息
set(CPACK_PACKAGE_NAME "CHTL")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CHTL超文本语言编译器")
set(CPACK_PACKAGE_VENDOR "CHTL Team")

include(CPack)