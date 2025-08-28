# CompilerDispatcher子模块

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置UTF-8支持
add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)

# 查找源文件
file(GLOB_RECURSE SOURCES "*.cpp")

# 创建静态库
add_library(CompilerDispatcher STATIC ${SOURCES})

# 包含目录
target_include_directories(CompilerDispatcher PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接依赖库
target_link_libraries(CompilerDispatcher
    Scanner
    CHTLLexer
    CHTLContext
)