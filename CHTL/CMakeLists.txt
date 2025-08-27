cmake_minimum_required(VERSION 3.10)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 设置UTF-8支持
if(MSVC)
    add_compile_options(/utf-8)
endif()

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 添加子目录
add_subdirectory(CHTL)
add_subdirectory(CHTLJS)
add_subdirectory(CSS)
add_subdirectory(JS)
add_subdirectory(Scanner)
add_subdirectory(CompilerDispatcher)
add_subdirectory(Util)
add_subdirectory(ThirdParty)

# 主程序
add_executable(chtl_compiler 
    main.cpp
)

# 链接库
target_link_libraries(chtl_compiler
    chtl_core
    chtljs_core
    css_compiler
    js_compiler
    scanner
    compiler_dispatcher
    util
)

# 安装规则
install(TARGETS chtl_compiler
    RUNTIME DESTINATION bin
)

# 创建官方模块目录
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/module)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/module/CMOD)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/module/CJMOD)

# 启用测试
enable_testing()
add_subdirectory(Test)