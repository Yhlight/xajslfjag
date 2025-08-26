cmake_minimum_required(VERSION 3.16)

# 项目信息
project(CHTL
    VERSION 1.0.0
    DESCRIPTION "CHTL超文本语言编译器"
    LANGUAGES CXX
)

# C++标准设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# 默认构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/ANTLR4/include)
include_directories(${CMAKE_SOURCE_DIR}/ANTLR4DEBUG/include)

# 链接目录
link_directories(${CMAKE_SOURCE_DIR}/ANTLR4/lib)
link_directories(${CMAKE_SOURCE_DIR}/ANTLR4DEBUG/lib)

# 查找线程支持
find_package(Threads REQUIRED)

# ANTLR4运行时库
find_library(ANTLR4_RUNTIME_LIB antlr4-runtime PATHS ${CMAKE_SOURCE_DIR}/ANTLR4/lib)
if(NOT ANTLR4_RUNTIME_LIB)
    message(FATAL_ERROR "未找到ANTLR4运行时库")
endif()

# 源文件
set(CHTL_SOURCES
    # Token系统
    src/Token/CHTL/CHTLToken.cpp
    src/Token/CHTLJS/CHTLJSToken.cpp
    
    # 公共组件
    src/Common/GlobalMap.cpp
    src/Common/Context.cpp
    
    # AST系统
    src/AST/Common/ASTNode.cpp
    src/AST/CHTL/CHTLNodes.cpp
    src/AST/CHTLJS/CHTLJSNodes.cpp
    
    # 扫描器
    src/Scanner/CHTLUnifiedScanner.cpp
    
    # 核心组件
    src/Core/CompilerDispatcher.cpp
    
    # 编译器
    src/Compiler/CHTL/CHTLCompiler.cpp
    src/Compiler/CHTLJS/CHTLJSCompiler.cpp
    src/Compiler/CSS/CSSCompiler.cpp
    src/Compiler/JavaScript/JavaScriptCompiler.cpp
    
    # ANTLR生成的文件
    css/css3Lexer.cpp
    css/css3Parser.cpp
    css/css3ParserBaseListener.cpp
    css/css3ParserListener.cpp
    
    js/JavaScriptLexer.cpp
    js/JavaScriptParser.cpp
    js/JavaScriptParserBase.cpp
    js/JavaScriptParserBaseListener.cpp
    js/JavaScriptParserListener.cpp
)

# 头文件
set(CHTL_HEADERS
    src/Token/CHTL/CHTLToken.h
    src/Token/CHTLJS/CHTLJSToken.h
    src/Common/GlobalMap.h
    src/Common/Context.h
    src/AST/Common/ASTNode.h
    src/AST/CHTL/CHTLNodes.h
    src/AST/CHTLJS/CHTLJSNodes.h
    src/Scanner/CHTLUnifiedScanner.h
    src/Core/CompilerDispatcher.h
)

# 创建CHTL编译器库
add_library(chtl_lib STATIC ${CHTL_SOURCES})

# 设置库的属性
set_target_properties(chtl_lib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME "chtl"
)

# 链接库
target_link_libraries(chtl_lib
    ${ANTLR4_RUNTIME_LIB}
    Threads::Threads
)

# 创建CHTL编译器可执行文件
add_executable(chtl
    src/main.cpp
)

# 链接CHTL库
target_link_libraries(chtl chtl_lib)

# 创建测试可执行文件
add_executable(chtl_test
    tests/test_main.cpp
)

target_link_libraries(chtl_test chtl_lib)

# 安装规则
install(TARGETS chtl chtl_lib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES ${CHTL_HEADERS}
    DESTINATION include/chtl
)

# 安装ANTLR运行时
install(FILES ${CMAKE_SOURCE_DIR}/ANTLR4/lib/libantlr4-runtime.a
    DESTINATION lib
)

# 创建配置文件
configure_file(
    "${CMAKE_SOURCE_DIR}/config/CHTLConfig.h.in"
    "${CMAKE_BINARY_DIR}/CHTLConfig.h"
)

# 测试支持
enable_testing()

add_test(NAME chtl_basic_test COMMAND chtl_test)

# 文档生成（如果找到Doxygen）
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "生成API文档"
        VERBATIM
    )
endif()

# 打包配置
set(CPACK_GENERATOR "TGZ;ZIP")
set(CPACK_PACKAGE_NAME "CHTL")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "CHTL超文本语言编译器")
set(CPACK_PACKAGE_CONTACT "CHTL Team")

include(CPack)

# 显示配置信息
message(STATUS "CHTL配置信息:")
message(STATUS "  版本: ${PROJECT_VERSION}")
message(STATUS "  构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++标准: ${CMAKE_CXX_STANDARD}")
message(STATUS "  安装前缀: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  ANTLR4库: ${ANTLR4_RUNTIME_LIB}")

# 自定义目标
add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
)

# 代码格式化（如果找到clang-format）
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT} -i ${CHTL_SOURCES} ${CHTL_HEADERS}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "格式化代码"
    )
endif()

# 静态分析（如果找到cppcheck）
find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK} --enable=all --std=c++17 ${CMAKE_SOURCE_DIR}/src
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "运行静态分析"
    )
endif()