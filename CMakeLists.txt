cmake_minimum_required(VERSION 3.10)
project(CHTLProject)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置UTF-8编码支持
if(MSVC)
    add_compile_options(/utf-8)
else()
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 添加子目录 - 暂时只添加已实现的模块
add_subdirectory(CHTL)
# add_subdirectory("CHTL JS")
# add_subdirectory(CSS)
# add_subdirectory(JS)
add_subdirectory(Scanner)
add_subdirectory(CompilerDispatcher)
# add_subdirectory(ThirdParty)
# add_subdirectory(Util)
add_subdirectory(Test)
# add_subdirectory(Module)

# 设置全局包含目录
include_directories(${CMAKE_SOURCE_DIR})
# include_directories(${CMAKE_SOURCE_DIR}/ThirdParty)
# include_directories(${CMAKE_SOURCE_DIR}/ANTLR4/include)
# include_directories(${CMAKE_SOURCE_DIR}/css)
# include_directories(${CMAKE_SOURCE_DIR}/js)

# 链接ANTLR4库
# link_directories(${CMAKE_SOURCE_DIR}/ANTLR4/lib)
# link_directories(${CMAKE_SOURCE_DIR}/css)
# link_directories(${CMAKE_SOURCE_DIR}/js)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        add_compile_options(/Zi)
    else()
        add_compile_options(-g)
    endif()
endif()