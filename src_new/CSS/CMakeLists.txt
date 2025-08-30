# CSS编译器CMake配置

set(CSS_COMPILER_SOURCES
    CSSCompiler.cpp
)

set(CSS_COMPILER_HEADERS
    CSSCompiler.h
)

# 创建CSS编译器库
add_library(CSSCompiler STATIC
    ${CSS_COMPILER_SOURCES}
    ${CSS_COMPILER_HEADERS}
)

# 链接依赖
target_link_libraries(CSSCompiler
    Scanner
    CHTLError
    ANTLRCSSParser
    ANTLR4Runtime
)

# 设置包含目录
target_include_directories(CSSCompiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)