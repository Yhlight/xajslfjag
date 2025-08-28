cmake_minimum_required(VERSION 3.16)

project(CHTL 
    VERSION 1.0.0
    DESCRIPTION "CHTL Compiler Project - 完全模块化的CHTL和CHTL JS编译器"
    LANGUAGES CXX
)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加UTF-8支持
if(MSVC)
    add_compile_options(/utf-8)
endif()

# 设置编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O3)
    endif()
elseif(MSVC)
    add_compile_options(/W4)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Od)
    else()
        add_compile_options(/O2)
    endif()
endif()

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/ThirdParty/include
)

# ANTLR4库目录
link_directories(
    ${CMAKE_SOURCE_DIR}/src/ThirdParty/lib
)

# 创建CHTL编译器库
set(CHTL_SOURCES
    src/CHTL/CHTLLexer/CHTLToken.cpp
    src/CHTL/CHTLLexer/CHTLGlobalMap.cpp
    src/CHTL/CHTLLexer/CHTLLexer.cpp
)

add_library(CHTLCompiler STATIC ${CHTL_SOURCES})

# 创建CHTL JS编译器库
set(CHTLJS_SOURCES
    src/CHTLJS/CHTLJSLexer/CHTLJSToken.cpp
    src/CHTLJS/CHTLJSLexer/CHTLJSGlobalMap.cpp
)

add_library(CHTLJSCompiler STATIC ${CHTLJS_SOURCES})

# 创建统一扫描器库
set(SCANNER_SOURCES
    src/Scanner/CHTLUnifiedScanner.cpp
)

add_library(CHTLScanner STATIC ${SCANNER_SOURCES})

# 创建编译器调度器库
set(DISPATCHER_SOURCES
    src/CompilerDispatcher/CHTLCompilerDispatcher.cpp
)

add_library(CHTLDispatcher STATIC ${DISPATCHER_SOURCES})

# CSS编译器(ANTLR4生成)
set(CSS_SOURCES
    src/CSS/css3Lexer.cpp
    src/CSS/css3Parser.cpp
    src/CSS/css3ParserBaseListener.cpp
    src/CSS/css3ParserListener.cpp
)

add_library(CSSCompiler STATIC ${CSS_SOURCES})

# JS编译器(ANTLR4生成)
set(JS_SOURCES
    src/JS/JavaScriptLexer.cpp
    src/JS/JavaScriptParser.cpp
    src/JS/JavaScriptParserBaseListener.cpp
    src/JS/JavaScriptParserListener.cpp
    src/JS/JavaScriptLexerBase.cpp
    src/JS/JavaScriptParserBase.cpp
)

add_library(JSCompiler STATIC ${JS_SOURCES})

# 链接ANTLR4运行时库 - 暂时注释，待ANTLR4库配置完成后启用
# target_link_libraries(CSSCompiler antlr4-runtime)
# target_link_libraries(JSCompiler antlr4-runtime)

# 创建主可执行文件
add_executable(chtl-compiler
    src/main.cpp
)

# 链接所有库 - 调整链接顺序，CHTLDispatcher依赖CHTLScanner
target_link_libraries(chtl-compiler
    CHTLDispatcher
    CHTLScanner
    CHTLCompiler
    CHTLJSCompiler
    # CSSCompiler    # 暂时注释，待ANTLR4配置完成后启用
    # JSCompiler     # 暂时注释，待ANTLR4配置完成后启用
)

# 测试可执行文件
add_executable(chtl-test
    src/Test/test_main.cpp
)

target_link_libraries(chtl-test
    CHTLCompiler
    CHTLJSCompiler
    CHTLScanner
    CHTLDispatcher
)

# 安装规则
install(TARGETS chtl-compiler
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/Module/
    DESTINATION module
    FILES_MATCHING PATTERN "*.cmod" PATTERN "*.cjmod" PATTERN "*.chtl"
)

# 创建module目录
install(CODE "file(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/module)")

# 输出编译信息
message(STATUS "CHTL Compiler Configuration:")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  CXX Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Source Dir: ${CMAKE_SOURCE_DIR}")
message(STATUS "")
message(STATUS "CHTL项目架构:")
message(STATUS "  CHTL编译器: 完全独立的CHTL语法处理")
message(STATUS "  CHTL JS编译器: 完全独立的CHTL JS语法处理")  
message(STATUS "  统一扫描器: 精准代码切割")
message(STATUS "  编译器调度器: 协调各编译器工作")
message(STATUS "  CSS/JS编译器: ANTLR4实现")
message(STATUS "")