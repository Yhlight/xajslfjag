# CHTL JS节点CMake配置

set(CHTLJS_NODE_SOURCES
    BaseNode.cpp
)

set(CHTLJS_NODE_HEADERS
    BaseNode.h
)

# 创建CHTL JS节点库
add_library(CHTLJSNode STATIC
    ${CHTLJS_NODE_SOURCES}
    ${CHTLJS_NODE_HEADERS}
)

# 链接依赖
target_link_libraries(CHTLJSNode
    CHTLJSLexer
    CHTLError
)

# 设置包含目录
target_include_directories(CHTLJSNode PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)