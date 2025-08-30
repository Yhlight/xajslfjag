# CMOD系统CMake配置

set(CMOD_SOURCES
    CMODApi.cpp
)

set(CMOD_HEADERS
    CMODApi.h
)

# 创建CMOD系统库
add_library(CMODSystem STATIC
    ${CMOD_SOURCES}
    ${CMOD_HEADERS}
)

# 链接依赖
target_link_libraries(CMODSystem
    CHTLUtil
)

# 设置包含目录
target_include_directories(CMODSystem PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)