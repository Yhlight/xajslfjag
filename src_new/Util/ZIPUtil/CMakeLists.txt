# ZIP工具CMake配置

set(ZIPUTIL_SOURCES
    ZIPUtil.cpp
)

set(ZIPUTIL_HEADERS
    ZIPUtil.h
)

# 创建ZIP工具库
add_library(CHTLZIPUtil STATIC
    ${ZIPUTIL_SOURCES}
    ${ZIPUTIL_HEADERS}
)

# 设置包含目录
target_include_directories(CHTLZIPUtil PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)