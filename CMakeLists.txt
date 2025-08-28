cmake_minimum_required(VERSION 3.16)
project(CHTL_Compiler VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
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

# 查找ANTLR4
find_package(PkgConfig REQUIRED)
pkg_check_modules(ANTLR4 REQUIRED antlr4-runtime)

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/ANTLR4/include
    ${CMAKE_SOURCE_DIR}/css
    ${CMAKE_SOURCE_DIR}/js
)

# 链接目录
link_directories(
    ${CMAKE_SOURCE_DIR}/ANTLR4/lib
    ${CMAKE_SOURCE_DIR}/css
    ${CMAKE_SOURCE_DIR}/js
)

# 源文件
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/CHTL/*.cpp"
    "src/CHTLJS/*.cpp"
    "src/CSS/*.cpp"
    "src/JS/*.cpp"
    "src/Scanner/*.cpp"
    "src/Common/*.cpp"
)

# 创建可执行文件
add_executable(CHTL_Compiler ${SOURCES})

# 链接库
target_link_libraries(CHTL_Compiler
    ${ANTLR4_LIBRARIES}
    antlr4-runtime
    css3
    javascript
)

# 编译选项
target_compile_options(CHTL_Compiler PRIVATE
    ${ANTLR4_CFLAGS_OTHER}
    -Wall
    -Wextra
    -O2
)

# 安装规则
install(TARGETS CHTL_Compiler
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)