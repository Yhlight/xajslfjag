# 编译器时间监控CMake配置

set(MONITOR_SOURCES
    TimeMonitor.cpp
)

set(MONITOR_HEADERS
    TimeMonitor.h
)

# 创建编译器时间监控库
add_library(CHTLCompilerTimeMonitor STATIC
    ${MONITOR_SOURCES}
    ${MONITOR_HEADERS}
)

# 设置包含目录
target_include_directories(CHTLCompilerTimeMonitor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)