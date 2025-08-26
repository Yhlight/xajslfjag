cmake_minimum_required(VERSION 3.14)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O3)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Od)
    else()
        add_compile_options(/O2)
    endif()
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 添加include目录
include_directories(${CMAKE_SOURCE_DIR}/src)

# ANTLR4支持（如果存在）
if(EXISTS ${CMAKE_SOURCE_DIR}/ANTLR4)
    include_directories(${CMAKE_SOURCE_DIR}/ANTLR4/runtime/Cpp/runtime/src)
    add_subdirectory(ANTLR4/runtime/Cpp)
endif()

# 收集源文件
file(GLOB_RECURSE SCANNER_SOURCES 
    ${CMAKE_SOURCE_DIR}/src/scanner/*.cpp
    ${CMAKE_SOURCE_DIR}/src/scanner/*.h
)

file(GLOB_RECURSE DISPATCHER_SOURCES 
    ${CMAKE_SOURCE_DIR}/src/dispatcher/*.cpp
    ${CMAKE_SOURCE_DIR}/src/dispatcher/*.h
)

file(GLOB_RECURSE CHTL_SOURCES 
    ${CMAKE_SOURCE_DIR}/src/chtl/*.cpp
    ${CMAKE_SOURCE_DIR}/src/chtl/*.h
)

file(GLOB_RECURSE CHTLJS_SOURCES 
    ${CMAKE_SOURCE_DIR}/src/chtljs/*.cpp
    ${CMAKE_SOURCE_DIR}/src/chtljs/*.h
)

file(GLOB_RECURSE COMMON_SOURCES 
    ${CMAKE_SOURCE_DIR}/src/common/*.cpp
    ${CMAKE_SOURCE_DIR}/src/common/*.h
)

# CSS和JS编译器源文件（ANTLR生成）
file(GLOB CSS_SOURCES 
    ${CMAKE_SOURCE_DIR}/css/*.cpp
    ${CMAKE_SOURCE_DIR}/css/*.h
)

file(GLOB JS_SOURCES 
    ${CMAKE_SOURCE_DIR}/js/*.cpp
    ${CMAKE_SOURCE_DIR}/js/*.h
)

# 创建核心库
add_library(chtl_core STATIC
    ${SCANNER_SOURCES}
    ${DISPATCHER_SOURCES}
    ${CHTL_SOURCES}
    ${CHTLJS_SOURCES}
    ${COMMON_SOURCES}
)

# 创建CSS编译器库
if(CSS_SOURCES)
    add_library(chtl_css STATIC ${CSS_SOURCES})
    target_link_libraries(chtl_css antlr4_static)
endif()

# 创建JS编译器库
if(JS_SOURCES)
    add_library(chtl_js STATIC ${JS_SOURCES})
    target_link_libraries(chtl_js antlr4_static)
endif()

# 创建主编译器可执行文件
add_executable(chtl
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)

# 链接库
target_link_libraries(chtl chtl_core)
if(TARGET chtl_css)
    target_link_libraries(chtl chtl_css)
endif()
if(TARGET chtl_js)
    target_link_libraries(chtl chtl_js)
endif()

# 创建测试目录
enable_testing()
add_subdirectory(test EXCLUDE_FROM_ALL)

# 安装规则
install(TARGETS chtl DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/modules/CMOD DESTINATION modules)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/modules/CJMOD DESTINATION modules)

# 打包配置
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)