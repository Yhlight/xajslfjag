# CHTL词法分析器CMake配置

set(CHTL_LEXER_SOURCES
    Lexer.cpp
    Token.cpp
    GlobalMap.cpp
    Type.cpp
)

set(CHTL_LEXER_HEADERS
    Lexer.h
    Token.h
    GlobalMap.h
    Type.h
)

# 创建CHTL词法分析器库
add_library(CHTLLexer STATIC
    ${CHTL_LEXER_SOURCES}
    ${CHTL_LEXER_HEADERS}
)

# 链接依赖
target_link_libraries(CHTLLexer
    CHTLState
    CHTLContext
    CHTLError
)

# 设置包含目录
target_include_directories(CHTLLexer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)