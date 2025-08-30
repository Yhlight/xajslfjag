# CHTL JS上下文管理CMake配置

set(CHTLJS_CONTEXT_SOURCES
    Context.cpp
)

set(CHTLJS_CONTEXT_HEADERS
    Context.h
)

# 创建CHTL JS上下文管理库
add_library(CHTLJSContext STATIC
    ${CHTLJS_CONTEXT_SOURCES}
    ${CHTLJS_CONTEXT_HEADERS}
)

# 链接依赖
target_link_libraries(CHTLJSContext
    CHTLJSLexer
    CHTLJSState
)

# 设置包含目录
target_include_directories(CHTLJSContext PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)