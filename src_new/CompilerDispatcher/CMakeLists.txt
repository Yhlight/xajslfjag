# 编译器调度器CMake配置

set(COMPILER_DISPATCHER_SOURCES
    CompilerDispatcher.cpp
)

set(COMPILER_DISPATCHER_HEADERS
    CompilerDispatcher.h
)

# 创建编译器调度器库
add_library(CompilerDispatcher STATIC
    ${COMPILER_DISPATCHER_SOURCES}
    ${COMPILER_DISPATCHER_HEADERS}
)

# 链接依赖
target_link_libraries(CompilerDispatcher
    Scanner
    CHTLError
)

# 设置包含目录
target_include_directories(CompilerDispatcher PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)