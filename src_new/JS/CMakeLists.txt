# JavaScript编译器CMake配置

set(JS_COMPILER_SOURCES
    JSCompiler.cpp
)

set(JS_COMPILER_HEADERS
    JSCompiler.h
)

# 创建JavaScript编译器库
add_library(JSCompiler STATIC
    ${JS_COMPILER_SOURCES}
    ${JS_COMPILER_HEADERS}
)

# 链接依赖
target_link_libraries(JSCompiler
    Scanner
    CHTLError
    ANTLRJSParser
    ANTLR4Runtime
)

# 设置包含目录
target_include_directories(JSCompiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)