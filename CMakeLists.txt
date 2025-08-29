cmake_minimum_required(VERSION 3.15)

# 项目配置
project(CHTL 
    VERSION 1.0.0
    DESCRIPTION "CHTL超文本语言编译器"
    LANGUAGES CXX
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 编译选项
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找依赖
find_package(Threads REQUIRED)

# 查找ANTLR4
find_path(ANTLR4_INCLUDE_DIR
    NAMES antlr4-runtime.h
    PATHS 
        ${CMAKE_SOURCE_DIR}/ANTLR4/include
        ${CMAKE_SOURCE_DIR}/ANTLR4DEBUG/include
        /usr/local/include/antlr4-runtime
        /usr/include/antlr4-runtime
)

find_library(ANTLR4_LIBRARY
    NAMES antlr4-runtime
    PATHS
        ${CMAKE_SOURCE_DIR}/ANTLR4/lib
        ${CMAKE_SOURCE_DIR}/ANTLR4DEBUG/lib
        /usr/local/lib
        /usr/lib
)

if(ANTLR4_INCLUDE_DIR AND ANTLR4_LIBRARY)
    message(STATUS "找到ANTLR4: ${ANTLR4_LIBRARY}")
    set(ANTLR4_FOUND TRUE)
else()
    message(WARNING "未找到ANTLR4，将使用内置副本")
    set(ANTLR4_FOUND FALSE)
endif()

# 头文件目录
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/Util
)

if(ANTLR4_FOUND)
    include_directories(${ANTLR4_INCLUDE_DIR})
endif()

# 源文件列表
set(UTIL_SOURCES
    src/Util/Common.cpp
)

set(SCANNER_SOURCES
    src/Scanner/CHTLUnifiedScanner.cpp
)

set(CHTL_LEXER_SOURCES
    src/CHTL/CHTLLexer/Token.cpp
    src/CHTL/CHTLLexer/GlobalMap.cpp
    src/CHTL/CHTLLexer/Lexer.cpp
)

set(CHTL_STATE_SOURCES
    src/CHTL/CHTLState/State.cpp
)

set(CHTL_CONTEXT_SOURCES
    src/CHTL/CHTLContext/Context.cpp
)

set(CHTL_NODE_SOURCES
    src/CHTL/CHTLNode/BaseNode.cpp
)

# CSS和JavaScript解析器（ANTLR生成的文件）
set(CSS_PARSER_SOURCES
    css/css3Lexer.cpp
    css/css3Parser.cpp
    css/css3ParserBaseListener.cpp
    css/css3ParserListener.cpp
)

set(JS_PARSER_SOURCES
    js/JavaScriptLexer.cpp
    js/JavaScriptLexerBase.cpp
    js/JavaScriptParser.cpp
    js/JavaScriptParserBase.cpp
    js/JavaScriptParserBaseListener.cpp
    js/JavaScriptParserListener.cpp
)

# 所有核心源文件
set(CORE_SOURCES
    ${UTIL_SOURCES}
    ${SCANNER_SOURCES}
    ${CHTL_LEXER_SOURCES}
    ${CHTL_STATE_SOURCES}
    ${CHTL_CONTEXT_SOURCES}
    ${CHTL_NODE_SOURCES}
)

# 条件包含ANTLR生成的源文件
if(ANTLR4_FOUND)
    list(APPEND CORE_SOURCES ${CSS_PARSER_SOURCES} ${JS_PARSER_SOURCES})
endif()

# 主要可执行文件
add_executable(chtl
    src/main.cpp
    ${CORE_SOURCES}
)

# 链接库
target_link_libraries(chtl 
    Threads::Threads
)

if(ANTLR4_FOUND)
    target_link_libraries(chtl ${ANTLR4_LIBRARY})
endif()

# 编译器特定设置
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(chtl PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
        $<$<CONFIG:Debug>:-fsanitize=address>
        $<$<CONFIG:Debug>:-fsanitize=undefined>
    )
    
    target_link_options(chtl PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address>
        $<$<CONFIG:Debug>:-fsanitize=undefined>
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(chtl PRIVATE
        /W4
        /permissive-
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Release>:/O2>
    )
endif()

# 预处理器定义
target_compile_definitions(chtl PRIVATE
    CHTL_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    CHTL_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    CHTL_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    $<$<CONFIG:Debug>:CHTL_DEBUG>
    $<$<CONFIG:Release>:CHTL_RELEASE>
)

# 安装规则
install(TARGETS chtl
    RUNTIME DESTINATION bin
)

# 安装头文件（供扩展开发使用）
install(DIRECTORY src/
    DESTINATION include/chtl
    FILES_MATCHING PATTERN "*.h"
)

# 测试配置
option(BUILD_TESTS "构建测试" OFF)

if(BUILD_TESTS)
    enable_testing()
    
    # 查找测试框架
    find_package(GTest QUIET)
    
    if(GTest_FOUND)
        add_subdirectory(src/Test)
    else()
        message(WARNING "未找到Google Test，跳过测试构建")
    endif()
endif()

# 文档生成
option(BUILD_DOCS "构建文档" OFF)

if(BUILD_DOCS)
    find_package(Doxygen QUIET)
    
    if(Doxygen_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        add_custom_target(docs ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "生成API文档"
            VERBATIM
        )
    else()
        message(WARNING "未找到Doxygen，跳过文档生成")
    endif()
endif()

# 包配置
option(BUILD_PACKAGE "构建安装包" OFF)

if(BUILD_PACKAGE)
    include(CPack)
    
    set(CPACK_PACKAGE_NAME "CHTL")
    set(CPACK_PACKAGE_VENDOR "CHTL Team")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CHTL超文本语言编译器")
    set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    
    # Windows特定设置
    if(WIN32)
        set(CPACK_GENERATOR "NSIS")
        set(CPACK_NSIS_DISPLAY_NAME "CHTL编译器")
        set(CPACK_NSIS_HELP_LINK "https://github.com/CHTL-Team/CHTL")
        set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/CHTL-Team/CHTL")
        set(CPACK_NSIS_CONTACT "chtl-team@example.com")
    endif()
    
    # Linux特定设置
    if(UNIX AND NOT APPLE)
        set(CPACK_GENERATOR "DEB;RPM")
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "CHTL Team <chtl-team@example.com>")
        set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
        set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
        set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
        set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    endif()
    
    # macOS特定设置
    if(APPLE)
        set(CPACK_GENERATOR "DragNDrop")
        set(CPACK_DMG_VOLUME_NAME "CHTL编译器")
    endif()
endif()

# 性能分析支持
option(ENABLE_PROFILING "启用性能分析" OFF)

if(ENABLE_PROFILING)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(chtl PRIVATE -pg)
        target_link_options(chtl PRIVATE -pg)
    endif()
endif()

# 代码覆盖率
option(ENABLE_COVERAGE "启用代码覆盖率" OFF)

if(ENABLE_COVERAGE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(chtl PRIVATE --coverage)
        target_link_options(chtl PRIVATE --coverage)
    endif()
endif()

# 静态分析
option(ENABLE_STATIC_ANALYSIS "启用静态分析" OFF)

if(ENABLE_STATIC_ANALYSIS)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        set_target_properties(chtl PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-checks=-*,readability-*,performance-*,modernize-*"
        )
    endif()
    
    find_program(CPPCHECK_EXE NAMES "cppcheck")
    if(CPPCHECK_EXE)
        add_custom_target(cppcheck
            COMMAND ${CPPCHECK_EXE} --enable=all --std=c++17 --project=${CMAKE_BINARY_DIR}/compile_commands.json
            COMMENT "运行cppcheck静态分析"
        )
    endif()
endif()

# 编译数据库
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 打印配置信息
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++编译器: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++标准: ${CMAKE_CXX_STANDARD}")
message(STATUS "ANTLR4支持: ${ANTLR4_FOUND}")
message(STATUS "构建测试: ${BUILD_TESTS}")
message(STATUS "构建文档: ${BUILD_DOCS}")
message(STATUS "构建包: ${BUILD_PACKAGE}")
message(STATUS "性能分析: ${ENABLE_PROFILING}")
message(STATUS "代码覆盖率: ${ENABLE_COVERAGE}")
message(STATUS "静态分析: ${ENABLE_STATIC_ANALYSIS}")

# 自定义目标
add_custom_target(format
    COMMAND find src -name "*.cpp" -o -name "*.h" | xargs clang-format -i
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "格式化代码"
)

add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/lib
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "清理所有构建文件"
)