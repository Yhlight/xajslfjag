# Token测试CMake配置

set(TOKEN_TEST_SOURCES
    TokenPrint.cpp
    TokenTable.cpp
)

set(TOKEN_TEST_HEADERS
    TokenPrint.h
    TokenTable.h
)

# 创建Token测试库
add_library(CHTLTokenTest STATIC
    ${TOKEN_TEST_SOURCES}
    ${TOKEN_TEST_HEADERS}
)

# 设置包含目录
target_include_directories(CHTLTokenTest PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)