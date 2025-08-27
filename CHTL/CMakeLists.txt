cmake_minimum_required(VERSION 3.16)
project(CHTL)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 包含目录
include_directories(.)

# 收集源文件
file(GLOB_RECURSE CHTL_SOURCES 
    "CHTL/CHTLLexer/*.cpp"
    "CHTL/CHTLNode/*.cpp"
    "CHTL/CHTLContext/*.cpp"
    "CHTL JS/CHTLJSLexer/*.cpp"
    "Scanner/*.cpp"
)

file(GLOB_RECURSE CHTL_HEADERS
    "CHTL/CHTLLexer/*.h"
    "CHTL/CHTLNode/*.h"
    "CHTL/CHTLContext/*.h"
    "CHTL JS/CHTLJSLexer/*.h"
    "Scanner/*.h"
)

# 创建CHTL编译器库
add_library(chtl_compiler STATIC ${CHTL_SOURCES} ${CHTL_HEADERS})

# 设置库的包含目录
target_include_directories(chtl_compiler PUBLIC .)

# 如果是调试模式，启用调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(chtl_compiler PRIVATE DEBUG_MODE=1)
endif()

# 创建测试可执行文件（可选）
option(BUILD_TESTS "Build test executables" ON)
if(BUILD_TESTS)
    add_executable(chtl_test Test/test_main.cpp)
    target_link_libraries(chtl_test chtl_compiler)
    target_include_directories(chtl_test PRIVATE .)
endif()

# 安装规则
install(TARGETS chtl_compiler
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY CHTL/
    DESTINATION include/CHTL
    FILES_MATCHING PATTERN "*.h"
)