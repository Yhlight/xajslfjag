cmake_minimum_required(VERSION 3.10)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译配置信息
message(STATUS "CHTL Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")

# 设置源文件 - 只包含确定能编译的核心文件
set(CHTL_CORE_SOURCES
    # Test Framework - 基础部分
    src/Test/TokenTest/TokenPrint.cpp
    src/Test/ASTTest/ASTPrint.cpp
    src/Test/CompileTimer/CompileTimer.cpp
    
    # CHTL Core - 核心组件
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
    src/CHTL/CHTLNode/ImportNode.cpp
    src/CHTL/CHTLConfig/ConfigurationManager.cpp
    src/CHTL/CHTLLoader/ImportEnhancer.cpp
    src/CHTL/CMODSystem/CMODManager.cpp
    src/CHTL/CHTLNode/InheritanceManager.cpp
    src/CHTL/CHTLGenerator/Generator.cpp
    src/CHTL/CHTLState/State.cpp
    src/CHTL/CHTLContext/Context.cpp
    src/CHTL/CHTLIOStream/FileStream.cpp
    src/CHTL/CHTLSelector/SelectorAutomation.cpp
    src/CHTL/CHTLNamespace/NamespaceManager.cpp
    src/CHTL/CHTLConstraint/ConstraintSystem.cpp
    
    # CHTL JS Lexer - 基础功能
    src/CHTLJS/CHTLJSLexer/Token.cpp
    src/CHTLJS/CHTLJSLexer/GlobalMap.cpp
    src/CHTLJS/CHTLJSLexer/Lexer.cpp
    src/CHTLJS/CHTLJSIOStream/JSFileStream.cpp
    src/CHTLJS/CHTLJSParser/EnhancedParser.cpp
    
    # CMOD System - 核心API
    src/CMODSystem/Syntax.cpp
    src/CMODSystem/CJMODGenerator.cpp
    
    # Util System
    src/Util/FileSystem/FileSystem.cpp
    
    # Error System
    src/Error/ErrorManager.cpp
    
    # Scanner System
    src/Scanner/UnifiedScanner.cpp
    
    # Compiler Dispatcher
    src/CompilerDispatcher/Dispatcher.cpp
    
    # Module System
    src/Module/ModuleManager.cpp
    
    # Test System
    src/Test/UtilTest/UtilTestSuite.cpp
)

# 设置包含目录
set(CHTL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Util
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTL
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTLJS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CJMOD
)

# 创建CHTL核心库
add_library(chtl_core STATIC ${CHTL_CORE_SOURCES})

# 设置包含目录
target_include_directories(chtl_core 
    PUBLIC ${CHTL_INCLUDE_DIRS}
)

# 设置编译器特定的选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(chtl_core PRIVATE
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter  # 暂时忽略未使用参数警告
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(chtl_core PRIVATE
        /W4
        /wd4100  # 暂时忽略未使用参数警告
    )
endif()

# 设置调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(chtl_core PRIVATE CHTL_DEBUG=1)
endif()

# 创建示例可执行文件（可选）
option(BUILD_EXAMPLES "Build example programs" OFF)
if(BUILD_EXAMPLES)
    add_executable(chtl_example
        examples/main.cpp
    )
    target_link_libraries(chtl_example chtl_core)
endif()

# 安装配置
install(TARGETS chtl_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/
    DESTINATION include/chtl
    FILES_MATCHING PATTERN "*.h"
)

# 显示构建信息
message(STATUS "CHTL build configuration complete")
message(STATUS "Core library will be built with ${CMAKE_CXX_COMPILER_ID} compiler")