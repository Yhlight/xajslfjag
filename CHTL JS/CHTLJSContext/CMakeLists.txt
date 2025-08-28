# CHTL JS Context CMake配置

# 设置源文件
set(CHTLJSCONTEXT_SOURCES
    CHTLJSContext.cpp
)

# 设置头文件
set(CHTLJSCONTEXT_HEADERS
    CHTLJSContext.h
)

# 创建静态库
add_library(CHTLJSContext STATIC ${CHTLJSCONTEXT_SOURCES})

# 设置目标属性
set_target_properties(CHTLJSContext PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

# 设置包含目录
target_include_directories(CHTLJSContext PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置编译选项
target_compile_options(CHTLJSContext PRIVATE
    -Wall
    -Wextra
    -fPIC
)