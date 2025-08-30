# CHTL JS解析器CMake配置

set(CHTLJS_PARSER_SOURCES
    CHTLJSParser.cpp
)

set(CHTLJS_PARSER_HEADERS
    CHTLJSParser.h
)

# 创建CHTL JS解析器库
add_library(CHTLJSParser STATIC
    ${CHTLJS_PARSER_SOURCES}
    ${CHTLJS_PARSER_HEADERS}
)

# 链接依赖
target_link_libraries(CHTLJSParser
    CHTLJSLexer
    CHTLJSNode
    CHTLJSState
    CHTLJSContext
    CHTLError
)

# 设置包含目录
target_include_directories(CHTLJSParser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)