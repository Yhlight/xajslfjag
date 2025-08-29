# CMakeLists.txt for CHTL Lexer
cmake_minimum_required(VERSION 3.16)

# 设置项目信息
project(CHTLLexer VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置源文件
set(CHTL_LEXER_SOURCES
    Token.cpp
    Type.cpp
    GlobalMap.cpp
    Lexer.cpp
)

set(CHTL_LEXER_HEADERS
    Token.h
    Type.h
    GlobalMap.h
    Lexer.h
)

# 创建静态库
add_library(CHTLLexer STATIC ${CHTL_LEXER_SOURCES} ${CHTL_LEXER_HEADERS})

# 设置包含目录
target_include_directories(CHTLLexer PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# 设置目标属性
set_target_properties(CHTLLexer PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${CHTL_LEXER_HEADERS}"
)

# 如果需要构建测试程序
option(BUILD_TESTS "Build test programs" ON)

if(BUILD_TESTS)
    # 创建简单的测试程序
    add_executable(test_lexer test_lexer.cpp)
    target_link_libraries(test_lexer CHTLLexer)
endif()