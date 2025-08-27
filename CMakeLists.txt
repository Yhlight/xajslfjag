cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置UTF-8编码支持
if(MSVC)
    add_compile_options(/utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# 设置编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# 包含ANTLR4
set(ANTLR4_ROOT ${CMAKE_SOURCE_DIR}/ANTLR4)
set(ANTLR4_INCLUDE_DIR ${ANTLR4_ROOT}/include)
set(ANTLR4_LIB_DIR ${ANTLR4_ROOT}/lib)

# 添加ANTLR4头文件路径
include_directories(${ANTLR4_INCLUDE_DIR})

# 链接ANTLR4库
if(WIN32)
    link_directories(${ANTLR4_LIB_DIR})
    set(ANTLR4_LIBRARIES antlr4-runtime-static)
else()
    # 在Linux上暂时不链接ANTLR4库，因为提供的是Windows库
    # 在实际项目中，这里应该使用适当的Linux ANTLR4库
    set(ANTLR4_LIBRARIES "")
endif()

# 包含目录
include_directories(src)
include_directories(css)
include_directories(js)

# 添加子目录
add_subdirectory(src/CHTL)
add_subdirectory(src/CHTLJS)
add_subdirectory(src/CSS)
add_subdirectory(src/JS)
add_subdirectory(src/Scanner)
add_subdirectory(src/CompilerDispatcher)
add_subdirectory(src/Util)

# 主可执行文件
add_executable(chtl src/main.cpp)

# 链接所有模块
target_link_libraries(chtl
    CHtml_CHTL
    CHtml_CHTLJS
    CHtml_CSS
    CHtml_JS
    CHtml_Scanner
    CHtml_CompilerDispatcher
    CHtml_Util
    ${ANTLR4_LIBRARIES}
)

# 复制ANTLR4运行时DLL（Windows）
if(WIN32)
    add_custom_command(TARGET chtl POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ANTLR4_LIB_DIR}/antlr4-runtime.dll"
        $<TARGET_FILE_DIR:chtl>
    )
endif()