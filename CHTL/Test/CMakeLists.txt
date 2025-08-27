cmake_minimum_required(VERSION 3.16)

# 启用测试
enable_testing()

# 添加Google Test（可选）
find_package(GTest)

# 测试可执行文件
add_executable(test_lexer
    test_lexer.cpp
)

target_link_libraries(test_lexer
    PRIVATE
        CHTLCore
        CHTLUtil
)

add_executable(test_chtljs_lexer
    test_chtljs_lexer.cpp
)

target_link_libraries(test_chtljs_lexer
    PRIVATE
        CHTLJSCore
        CHTLUtil
)

# 添加测试
add_test(NAME LexerTest COMMAND test_lexer)
add_test(NAME CHTLJSLexerTest COMMAND test_chtljs_lexer)

# 测试文件目录
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_files
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})