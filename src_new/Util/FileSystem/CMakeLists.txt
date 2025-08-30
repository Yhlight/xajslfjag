# 文件系统工具CMake配置

set(FILESYSTEM_SOURCES
    FileSystem.cpp
)

set(FILESYSTEM_HEADERS
    FileSystem.h
)

# 创建文件系统工具库
add_library(CHTLFileSystem STATIC
    ${FILESYSTEM_SOURCES}
    ${FILESYSTEM_HEADERS}
)

# 设置包含目录
target_include_directories(CHTLFileSystem PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)