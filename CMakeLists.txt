cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置默认构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 设置UTF-8编码支持
if(MSVC)
    # Visual Studio UTF-8支持
    add_compile_options(/utf-8)
    add_compile_options(/W4)
    # 禁用特定警告
    add_compile_options(/wd4251 /wd4275)
else()
    # GCC/Clang UTF-8支持
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# 平台特定设置
if(WIN32)
    add_definitions(-DCHTL_PLATFORM_WINDOWS)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    # Windows UTF-8控制台支持
    add_definitions(-DUNICODE -D_UNICODE)
elseif(UNIX AND NOT APPLE)
    add_definitions(-DCHTL_PLATFORM_LINUX)
elseif(APPLE)
    add_definitions(-DCHTL_PLATFORM_MACOS)
endif()

# 添加包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# 查找线程库
find_package(Threads REQUIRED)

# ANTLR4运行时（暂时注释，后续集成）
# TODO: 集成ANTLR4
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ANTLR4/runtime/Cpp antlr4_runtime)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ANTLR4/runtime/Cpp/runtime/src)

# 添加源代码子目录
add_subdirectory(src)

# 测试目录（可选）
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    enable_testing()
    add_subdirectory(tests)
endif()

# 模块目录
if(EXISTS ${CMAKE_SOURCE_DIR}/Modules)
    add_subdirectory(Modules)
endif()

# 安装规则
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY module/ DESTINATION module)

# 创建模块输出目录
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/module)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/module/CMOD)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/module/CJMOD)

# 打印配置信息
message(STATUS "CHTL Compiler Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")