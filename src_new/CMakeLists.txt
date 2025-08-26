cmake_minimum_required(VERSION 3.16)
project(CHTL_Compiler VERSION 1.0.0 LANGUAGES CXX)

# C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译器特定设置
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 查找Java（用于ANTLR4）
find_package(Java REQUIRED)

# 设置ANTLR4路径
set(ANTLR4_JAR_PATH "${CMAKE_SOURCE_DIR}/../antlr-4.13.2-complete.jar")

# 生成ANTLR4 CSS解析器
set(CSS_GRAMMAR_DIR "${CMAKE_SOURCE_DIR}/../Grammars/CSS3")
set(CSS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/generated/css")

add_custom_command(
    OUTPUT ${CSS_OUTPUT_DIR}/css3Lexer.cpp
           ${CSS_OUTPUT_DIR}/css3Parser.cpp
           ${CSS_OUTPUT_DIR}/css3ParserBaseListener.cpp
    COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR4_JAR_PATH}
            -Dlanguage=Cpp -o ${CSS_OUTPUT_DIR}
            ${CSS_GRAMMAR_DIR}/css3Lexer.g4
            ${CSS_GRAMMAR_DIR}/css3Parser.g4
    DEPENDS ${CSS_GRAMMAR_DIR}/css3Lexer.g4
            ${CSS_GRAMMAR_DIR}/css3Parser.g4
    COMMENT "Generating CSS3 ANTLR4 files"
)

# 生成ANTLR4 JavaScript解析器
set(JS_GRAMMAR_DIR "${CMAKE_SOURCE_DIR}/../Grammars/JavaScript")
set(JS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/generated/js")

add_custom_command(
    OUTPUT ${JS_OUTPUT_DIR}/JavaScriptLexer.cpp
           ${JS_OUTPUT_DIR}/JavaScriptParser.cpp
           ${JS_OUTPUT_DIR}/JavaScriptParserBaseListener.cpp
    COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR4_JAR_PATH}
            -Dlanguage=Cpp -o ${JS_OUTPUT_DIR}
            ${JS_GRAMMAR_DIR}/JavaScriptLexer.g4
            ${JS_GRAMMAR_DIR}/JavaScriptParser.g4
    DEPENDS ${JS_GRAMMAR_DIR}/JavaScriptLexer.g4
            ${JS_GRAMMAR_DIR}/JavaScriptParser.g4
    COMMENT "Generating JavaScript ANTLR4 files"
)

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/generated
    ${CSS_OUTPUT_DIR}
    ${JS_OUTPUT_DIR}
)

# CHTL核心源文件
set(CHTL_SOURCES
    # CHTL词法分析器
    CHTL/CHTLLexer/CHTLLexer.h
    CHTL/CHTLLexer/GlobalMap.h
    CHTL/CHTLLexer/Token.h
    
    # CHTL节点
    CHTL/CHTLNode/BaseNode.h
    CHTL/CHTLNode/CommentNode.h
    CHTL/CHTLNode/TemplateNode.h
    CHTL/CHTLNode/CustomNode.h
    CHTL/CHTLNode/StyleNode.h
    CHTL/CHTLNode/ScriptNode.h
    CHTL/CHTLNode/OriginNode.h
    CHTL/CHTLNode/ImportNode.h
    CHTL/CHTLNode/ConfigNode.h
    CHTL/CHTLNode/NamespaceNode.h
    CHTL/CHTLNode/OperatorNode.h
    
    # CHTL解析器
    CHTL/CHTLParser/CHTLParser.h
    
    # CHTL生成器
    CHTL/CHTLGenerator/CHTLGenerator.h
    
    # CHTL上下文
    CHTL/CHTLContext/CHTLContext.h
    
    # CHTL加载器
    CHTL/CHTLLoader/CHTLLoader.h
    
    # CHTL状态机
    CHTL/CHTLState/CHTLState.h
    
    # CHTL管理器
    CHTL/CHTLManage/CHTLManage.h
    
    # CMOD系统
    CHTL/CMODSystem/CMODSystem.h
)

# CHTL JS源文件
set(CHTLJS_SOURCES
    # CHTL JS词法分析器
    CHTLJS/CHTLJSLexer/CHTLJSLexer.h
    CHTLJS/CHTLJSLexer/CHTLJSGlobalMap.h
    CHTLJS/CHTLJSLexer/CHTLJSToken.h
    
    # CHTL JS节点
    CHTLJS/CHTLJSNode/CHTLJSNodes.h
    
    # CHTL JS解析器
    CHTLJS/CHTLJSParser/CHTLJSParser.h
    
    # CHTL JS生成器
    CHTLJS/CHTLJSGenerator/CHTLJSGenerator.h
    
    # CHTL JS上下文
    CHTLJS/CHTLJSContext/CHTLJSContext.h
    
    # CHTL JS加载器
    CHTLJS/CHTLJSLoader/CHTLJSLoader.h
    
    # CHTL JS状态机
    CHTLJS/CHTLJSState/CHTLJSState.h
    
    # CHTL JS管理器
    CHTLJS/CHTLJSManage/CHTLJSManage.h
    
    # CJMOD系统
    CHTLJS/CJMODSystem/CJMODSystem.h
)

# CSS和JS编译器源文件
set(CSS_JS_SOURCES
    CSS/CSSCompiler.h
    JS/JavaScriptCompiler.h
)

# 扫描器源文件
set(SCANNER_SOURCES
    Scanner/CHTLUnifiedScanner.h
    Scanner/CJMODScanner.h
)

# 编译器调度器源文件
set(DISPATCHER_SOURCES
    CompilerDispatcher/CompilerDispatcher.h
)

# 工具类源文件
set(UTIL_SOURCES
    Util/FileSystem/FileSystem.h
    Util/FileSystem/ZipLibrary.h
)

# ANTLR生成的源文件
set(ANTLR_SOURCES
    ${CSS_OUTPUT_DIR}/css3Lexer.cpp
    ${CSS_OUTPUT_DIR}/css3Parser.cpp
    ${CSS_OUTPUT_DIR}/css3ParserBaseListener.cpp
    ${JS_OUTPUT_DIR}/JavaScriptLexer.cpp
    ${JS_OUTPUT_DIR}/JavaScriptParser.cpp
    ${JS_OUTPUT_DIR}/JavaScriptParserBaseListener.cpp
)

# 合并所有源文件
set(ALL_SOURCES
    ${CHTL_SOURCES}
    ${CHTLJS_SOURCES}
    ${CSS_JS_SOURCES}
    ${SCANNER_SOURCES}
    ${DISPATCHER_SOURCES}
    ${UTIL_SOURCES}
    ${ANTLR_SOURCES}
)

# 创建CHTL编译器库（仅头文件接口库）
add_library(chtl_compiler_lib INTERFACE)

# 设置INTERFACE库的包含目录
target_include_directories(chtl_compiler_lib INTERFACE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/generated
    ${CSS_OUTPUT_DIR}
    ${JS_OUTPUT_DIR}
)

# 创建可执行文件
add_executable(chtl_compiler Test/main.cpp)

# 创建CJMOD测试可执行文件
add_executable(test_cjmod Test/test_cjmod.cpp)

# 链接库
target_link_libraries(chtl_compiler chtl_compiler_lib)
target_link_libraries(test_cjmod chtl_compiler_lib)

# 设置可执行文件属性
set_target_properties(chtl_compiler PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(test_cjmod PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 为可执行文件单独设置C++标准
target_compile_features(chtl_compiler PRIVATE cxx_std_17)
target_compile_features(test_cjmod PRIVATE cxx_std_17)

# 安装目标
install(TARGETS chtl_compiler test_cjmod
        RUNTIME DESTINATION bin)

install(TARGETS chtl_compiler_lib
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

# 安装头文件
install(DIRECTORY CHTL/ DESTINATION include/CHTL
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY CHTLJS/ DESTINATION include/CHTLJS
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY CSS/ DESTINATION include/CSS
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY JS/ DESTINATION include/JS
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY Scanner/ DESTINATION include/Scanner
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY CompilerDispatcher/ DESTINATION include/CompilerDispatcher
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY Util/ DESTINATION include/Util
        FILES_MATCHING PATTERN "*.h")

# 模块安装
install(DIRECTORY Module/ DESTINATION share/chtl/modules
        PATTERN "*.chtl" PATTERN "*.h" PATTERN "*.cpp")

# 设置构建信息
configure_file(
    "${CMAKE_SOURCE_DIR}/../version.h.in"
    "${CMAKE_BINARY_DIR}/version.h"
    @ONLY
)

# 添加自定义目标用于清理ANTLR生成的文件
add_custom_target(clean-antlr
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CSS_OUTPUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${JS_OUTPUT_DIR}
    COMMENT "Cleaning ANTLR generated files"
)

# 打印配置信息
message(STATUS "CHTL Compiler Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Source Directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "  Binary Directory: ${CMAKE_BINARY_DIR}")
message(STATUS "  ANTLR4 JAR: ${ANTLR4_JAR_PATH}")
message(STATUS "  CSS Grammar: ${CSS_GRAMMAR_DIR}")
message(STATUS "  JS Grammar: ${JS_GRAMMAR_DIR}")

# 编译信息
message(STATUS "Components to build:")
message(STATUS "  - CHTL Core Compiler")
message(STATUS "  - CHTL JS Compiler") 
message(STATUS "  - CSS Compiler (ANTLR4)")
message(STATUS "  - JavaScript Compiler (ANTLR4)")
message(STATUS "  - Unified Scanner")
message(STATUS "  - Compiler Dispatcher")
message(STATUS "  - Module System")
message(STATUS "  - Test Executable")

# Debug信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build enabled")
    add_compile_definitions(CHTL_DEBUG=1)
else()
    message(STATUS "Release build enabled")
    add_compile_definitions(CHTL_RELEASE=1)
endif()

message(STATUS "🎉 CHTL编译器项目配置完成！")
message(STATUS "💾 使用 'make' 或 'cmake --build .' 来构建项目")
message(STATUS "🚀 运行 './chtl_compiler' 来启动CHTL编译器测试")