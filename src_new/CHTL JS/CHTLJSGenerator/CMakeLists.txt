# CHTL JS生成器CMake配置

set(CHTLJS_GENERATOR_SOURCES
    CHTLJSGenerator.cpp
)

set(CHTLJS_GENERATOR_HEADERS
    CHTLJSGenerator.h
)

# 创建CHTL JS生成器库
add_library(CHTLJSGenerator STATIC
    ${CHTLJS_GENERATOR_SOURCES}
    ${CHTLJS_GENERATOR_HEADERS}
)

# 链接依赖
target_link_libraries(CHTLJSGenerator
    CHTLJSNode
    CHTLError
)

# 设置包含目录
target_include_directories(CHTLJSGenerator PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)