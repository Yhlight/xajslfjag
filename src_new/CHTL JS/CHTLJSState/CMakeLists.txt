# CHTL JS状态管理CMake配置

set(CHTLJS_STATE_SOURCES
    State.cpp
)

set(CHTLJS_STATE_HEADERS
    State.h
)

# 创建CHTL JS状态管理库
add_library(CHTLJSState STATIC
    ${CHTLJS_STATE_SOURCES}
    ${CHTLJS_STATE_HEADERS}
)

# 设置包含目录
target_include_directories(CHTLJSState PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)