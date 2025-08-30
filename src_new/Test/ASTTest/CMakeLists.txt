# AST测试CMake配置

set(AST_TEST_SOURCES
    ASTPrint.cpp
    ASTGraph.cpp
)

set(AST_TEST_HEADERS
    ASTPrint.h
    ASTGraph.h
)

# 创建AST测试库
add_library(CHTLASTTest STATIC
    ${AST_TEST_SOURCES}
    ${AST_TEST_HEADERS}
)

# 设置包含目录
target_include_directories(CHTLASTTest PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)