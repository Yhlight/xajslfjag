cmake_minimum_required(VERSION 3.10)
project(CHTL CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
if(MSVC)
    add_compile_options(/utf-8)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ANTLR4/include
)

# CHTL Lexer库
add_library(CHTLLexer STATIC
    CHTL/CHTLLexer/Token.cpp
    CHTL/CHTLLexer/GlobalMap.cpp
    CHTL/CHTLLexer/Lexer.cpp
)

# 测试可执行文件
add_executable(TestLexer
    Test/TestLexer.cpp
)

# 链接库
target_link_libraries(TestLexer CHTLLexer)

# 输出目录
set_target_properties(TestLexer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)