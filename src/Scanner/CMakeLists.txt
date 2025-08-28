# CHTLUnifiedScanner统一扫描器构建配置

# 源文件
set(SCANNER_SOURCES
    CHTLUnifiedScanner.cpp
    CodeFragment.cpp
    SyntaxAnalyzer.cpp
    FragmentOptimizer.cpp
)

# 头文件
set(SCANNER_HEADERS
    CHTLUnifiedScanner.h
    CodeFragment.h
    SyntaxAnalyzer.h
    FragmentOptimizer.h
)

# 创建扫描器库
add_library(scanner STATIC ${SCANNER_SOURCES} ${SCANNER_HEADERS})

# 设置目标属性
set_target_properties(scanner PROPERTIES
    OUTPUT_NAME "scanner"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 包含目录
target_include_directories(scanner PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# 安装规则
install(TARGETS scanner
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${SCANNER_HEADERS}
    DESTINATION include/scanner
)