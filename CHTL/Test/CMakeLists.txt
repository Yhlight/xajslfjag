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

add_executable(test_unquoted_literals
    test_unquoted_literals.cpp
)

target_link_libraries(test_unquoted_literals
    PRIVATE
        CHTLCore
        CHTLUtil
)

add_executable(test_chtl_features
    test_chtl_features.cpp
)

target_link_libraries(test_chtl_features
    PRIVATE
        CHTLCore
        CHTLUtil
)

# 添加测试
add_test(NAME LexerTest COMMAND test_lexer)
add_test(NAME CHTLJSLexerTest COMMAND test_chtljs_lexer)
add_test(NAME UnquotedLiteralsTest COMMAND test_unquoted_literals)
add_test(NAME CHTLFeaturesTest COMMAND test_chtl_features)

# 测试文件目录
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_files
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})