# CJMOD系统CMake配置

set(CJMOD_SOURCES
    CJMODApi.cpp
)

set(CJMOD_HEADERS
    CJMODApi.h
)

# 创建CJMOD系统库
add_library(CJMODSystem STATIC
    ${CJMOD_SOURCES}
    ${CJMOD_HEADERS}
)

# 设置包含目录
target_include_directories(CJMODSystem PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)