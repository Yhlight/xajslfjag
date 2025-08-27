cmake_minimum_required(VERSION 3.16)
project(CHTL_Compiler VERSION 1.0.0 LANGUAGES CXX)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# CHTL编译器核心模块
file(GLOB_RECURSE CHTL_SOURCES 
    "CHTL/*.cpp"
    "CHTL/*.h"
)

# CHTL JS编译器模块
file(GLOB_RECURSE CHTL_JS_SOURCES 
    "CHTL_JS/*.cpp"
    "CHTL_JS/*.h"
)

# CSS编译器模块
file(GLOB_RECURSE CSS_SOURCES 
    "CSS/*.cpp"
    "CSS/*.h"
)

# JS编译器模块
file(GLOB_RECURSE JS_SOURCES 
    "JS/*.cpp"
    "JS/*.h"
)

# Scanner扫描器模块
file(GLOB_RECURSE SCANNER_SOURCES 
    "Scanner/*.cpp"
    "Scanner/*.h"
)

# CompilerDispatcher调度器模块
file(GLOB_RECURSE DISPATCHER_SOURCES 
    "CompilerDispatcher/*.cpp"
    "CompilerDispatcher/*.h"
)

# 工具模块
file(GLOB_RECURSE UTIL_SOURCES 
    "Util/*.cpp"
    "Util/*.h"
)

# 第三方库
file(GLOB_RECURSE THIRDPARTY_SOURCES 
    "ThirdParty/*.cpp"
    "ThirdParty/*.h"
)

# 创建可执行文件
add_executable(chtl_compiler
    ${CHTL_SOURCES}
    ${CHTL_JS_SOURCES}
    ${CSS_SOURCES}
    ${JS_SOURCES}
    ${SCANNER_SOURCES}
    ${DISPATCHER_SOURCES}
    ${UTIL_SOURCES}
    ${THIRDPARTY_SOURCES}
    main.cpp
)

# 创建CHTL JS测试可执行文件
add_executable(chtl_js_test
    ${CHTL_JS_SOURCES}
    test_chtl_js.cpp
)

# 测试可执行文件
file(GLOB_RECURSE TEST_SOURCES "Test/*.cpp")
if(TEST_SOURCES)
    add_executable(chtl_test ${TEST_SOURCES})
    target_link_libraries(chtl_test ${CMAKE_THREAD_LIBS_INIT})
endif()

# 设置输出目录
set_target_properties(chtl_compiler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(chtl_js_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(TEST_SOURCES)
    set_target_properties(chtl_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()