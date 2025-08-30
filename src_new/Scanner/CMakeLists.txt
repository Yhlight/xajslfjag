# 统一扫描器CMake配置

set(SCANNER_SOURCES
    UnifiedScanner.cpp
)

set(SCANNER_HEADERS
    UnifiedScanner.h
)

# 创建统一扫描器库
add_library(Scanner STATIC
    ${SCANNER_SOURCES}
    ${SCANNER_HEADERS}
)

# 链接依赖
target_link_libraries(Scanner
    CHTLError
)

# 设置包含目录
target_include_directories(Scanner PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)