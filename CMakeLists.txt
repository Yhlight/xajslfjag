cmake_minimum_required(VERSION 3.10)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找线程库
find_package(Threads REQUIRED)

# 定义源文件（只包含实际存在的文件）
set(CHTL_CORE_SOURCES
    # Util
    src/Util/Common.cpp
    src/Util/FileSystem/FileSystem.cpp
    
    # IO Streams
    src/CHTL/CHTLIOStream/FileStream.cpp
    src/CHTLJS/CHTLJSIOStream/JSFileStream.cpp
    
    # Error Management
    src/Error/ErrorManager.cpp
    
    # Module System
    src/Module/ModuleManager.cpp
    
    # Test Framework (基础组件)
    src/Test/UtilTest/UtilTestSuite.cpp
    src/Test/TokenTest/TokenPrint.cpp
    src/Test/ASTTest/ASTPrint.cpp
    src/Test/CompileTimer/CompileTimer.cpp
    
    # CHTL Core - 只包含存在的文件
    src/CHTL/CHTLLexer/Token.cpp
    src/CHTL/CHTLLexer/GlobalMap.cpp
    src/CHTL/CHTLLexer/Lexer.cpp
    src/CHTL/CHTLParser/Parser.cpp
    src/CHTL/CHTLNode/BaseNode.cpp
    src/CHTL/CHTLNode/TemplateNode.cpp
    src/CHTL/CHTLNode/CustomNode.cpp
    src/CHTL/CHTLNode/OriginNode.cpp
    src/CHTL/CHTLNode/ConstraintNode.cpp
    src/CHTL/CHTLNode/SpecializationNode.cpp
    src/CHTL/CHTLConfig/ConfigurationManager.cpp
    src/CHTL/CHTLLoader/ImportEnhancer.cpp
    src/CHTL/CMODSystem/CMODManager.cpp
    src/CHTL/CHTLNode/InheritanceManager.cpp
    src/CHTL/CHTLGenerator/Generator.cpp
    src/CHTL/CHTLLoader/ModuleLoader.cpp
    src/CHTL/CHTLManage/NamespaceResolver.cpp
    src/CHTL/CHTLManage/ConstraintValidator.cpp
    src/CHTL/CHTLState/RAIIManager.cpp
    src/CHTL/CHTLState/State.cpp
    
    # CHTL JS
    src/CHTLJS/CHTLJSParser/Parser.cpp
    src/CHTLJS/CHTLJSNode/ModuleNode.cpp
    
    # Scanner and Dispatcher
    src/Scanner/CHTLUnifiedScanner.cpp
    src/CompilerDispatcher/Dispatcher.cpp
    
    # CMOD System
    src/CMODSystem/CJMODScanner.cpp
    src/CMODSystem/Syntax.cpp
    src/CMODSystem/CJMODGenerator.cpp
    
    # CHTL JS Lexer
    src/CHTLJS/CHTLJSLexer/Token.cpp
    src/CHTLJS/CHTLJSLexer/GlobalMap.cpp
    src/CHTLJS/CHTLJSLexer/Lexer.cpp
    
    # CHTL JS Generator
    src/CHTLJS/CHTLJSGenerator/Generator.cpp
    
    # CSS Compiler
    src/CSS/CSSCompiler.cpp
    
    # JS Compiler
    src/JS/JSCompiler.cpp
    
    # ZIP Utility
    src/Util/ZIPUtil/ZIPManager.cpp
    
    # ANTLR Integration
    src/ANTLR/ANTLRJavaScriptWrapper.cpp
    src/ANTLR/ANTLRCSSWrapper.cpp
    
    # ANTLR Generated JavaScript Files
    js/JavaScriptLexer.cpp
    js/JavaScriptParser.cpp
    js/JavaScriptParserBase.cpp
    js/JavaScriptLexerBase.cpp
    js/JavaScriptParserBaseListener.cpp
    js/JavaScriptParserListener.cpp
    
    # ANTLR Generated CSS Files
    css/css3Lexer.cpp
    css/css3Parser.cpp
    css/css3ParserBaseListener.cpp
    css/css3ParserListener.cpp
)

# 包含目录
set(CHTL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/ANTLR4/include
    ${CMAKE_CURRENT_SOURCE_DIR}/js
    ${CMAKE_CURRENT_SOURCE_DIR}/css
)

# 创建CHTL核心库
add_library(chtl_core STATIC ${CHTL_CORE_SOURCES})

# 设置包含目录
target_include_directories(chtl_core PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ANTLR4/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/js>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/css>
    $<INSTALL_INTERFACE:include>
)

# 链接库
target_link_libraries(chtl_core PUBLIC 
    Threads::Threads
    ${CMAKE_CURRENT_SOURCE_DIR}/ANTLR4/lib/antlr4-runtime-static.lib
)

# 设置编译器特定选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(chtl_core PRIVATE -Wall -Wextra -O2)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(chtl_core PRIVATE /W4 /O2)
endif()

# 创建CHTL编译器可执行文件
add_executable(chtl_compiler src/main.cpp)
target_link_libraries(chtl_compiler PRIVATE chtl_core)

# 创建测试可执行文件
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test_complete_integration.cpp")
    add_executable(test_complete_integration test_complete_integration.cpp)
    target_link_libraries(test_complete_integration PRIVATE chtl_core)
    target_include_directories(test_complete_integration PRIVATE ${CHTL_INCLUDE_DIRS})
endif()

# 显示配置信息
message(STATUS "CHTL Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")

# 安装规则
install(TARGETS chtl_core chtl_compiler
    EXPORT CHTLTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# 安装头文件
install(DIRECTORY src/ DESTINATION include/chtl
    FILES_MATCHING PATTERN "*.h"
)

# 导出配置
install(EXPORT CHTLTargets
    FILE CHTLTargets.cmake
    NAMESPACE CHTL::
    DESTINATION lib/cmake/CHTL
)