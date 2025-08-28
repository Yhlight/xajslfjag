# Scanner模块 - 统一扫描器

# 设置Scanner源文件
set(SCANNER_SOURCES
    CHTLUnifiedScanner.cpp
    # ScannerContext.cpp
    # ScannerState.cpp
)

# 创建Scanner静态库
add_library(Scanner STATIC ${SCANNER_SOURCES})

# 设置目标属性
set_target_properties(Scanner PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接依赖库 - 暂时不链接其他库
# target_link_libraries(Scanner
#     CHTL
#     CHTLJS
# )