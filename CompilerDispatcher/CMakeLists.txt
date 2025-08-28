# CompilerDispatcher模块 - 编译器调度器

# 设置CompilerDispatcher源文件
set(COMPILER_DISPATCHER_SOURCES
    CompilerDispatcher.cpp
    CompilerManager.cpp
    ResultMerger.cpp
)

# 创建CompilerDispatcher静态库
add_library(CompilerDispatcher STATIC ${COMPILER_DISPATCHER_SOURCES})

# 设置目标属性
set_target_properties(CompilerDispatcher PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接依赖库
target_link_libraries(CompilerDispatcher
    Scanner
    CHTL
    CHTLJS
    CSS
    JS
)