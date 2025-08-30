# CHTL JS词法分析器CMake配置

set(CHTLJS_LEXER_SOURCES
    Lexer.cpp
    Token.cpp
    GlobalMap.cpp
)

set(CHTLJS_LEXER_HEADERS
    Lexer.h
    Token.h
    GlobalMap.h
)

# 创建CHTL JS词法分析器库
add_library(CHTLJSLexer STATIC
    ${CHTLJS_LEXER_SOURCES}
    ${CHTLJS_LEXER_HEADERS}
)

# 链接依赖
target_link_libraries(CHTLJSLexer
    CHTLJSState
    CHTLJSContext
    CHTLError
)

# 设置包含目录
target_include_directories(CHTLJSLexer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)