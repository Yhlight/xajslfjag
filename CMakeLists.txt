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

# 添加子目录
add_subdirectory(Scanner)
add_subdirectory(Test)
add_subdirectory(CHTL)
add_subdirectory(CHTL JS)
add_subdirectory(CompilerDispatcher)

# 添加调试测试
add_executable(DebugTest Test/debug_test.cpp)
set_target_properties(DebugTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(DebugTest CHTLParser CHTLLexer CHTLJSParser CHTLJSLexer CHTLContext Scanner CompilerDispatcher)
target_include_directories(DebugTest PRIVATE ${CMAKE_SOURCE_DIR})

# 添加简化测试
add_executable(SimpleTest Test/simple_test.cpp)
set_target_properties(SimpleTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(SimpleTest CHTLParser CHTLLexer CHTLJSParser CHTLJSLexer CHTLContext Scanner CompilerDispatcher)
target_include_directories(SimpleTest PRIVATE ${CMAKE_SOURCE_DIR})

# 添加生成器测试
add_executable(GeneratorTest Test/generator_test.cpp)
set_target_properties(GeneratorTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(GeneratorTest CHTLParser CHTLLexer CHTLJSParser CHTLJSLexer CHTLContext Scanner CompilerDispatcher CHTLGenerator CHTLJSGenerator)
target_include_directories(GeneratorTest PRIVATE ${CMAKE_SOURCE_DIR})

# 创建CHTLLexer静态库
set(CHTLLEXER_SOURCES
    CHTL/CHTLLexer/CHTLToken.cpp
    CHTL/CHTLLexer/CHTLGlobalMap.cpp
    CHTL/CHTLLexer/CHTLLexer.cpp
)

add_library(CHTLLexer STATIC ${CHTLLEXER_SOURCES})

# 设置目标属性
set_target_properties(CHTLLexer PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 创建CHTLContext静态库
set(CHTLCONTEXT_SOURCES
    CHTL/CHTLContext/ImportManager.cpp
    CHTL/CHTLContext/SelectorAutomation.cpp
    CHTL/CHTLContext/NamespaceManager.cpp
)

add_library(CHTLContext STATIC ${CHTLCONTEXT_SOURCES})

# 设置目标属性
set_target_properties(CHTLContext PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 设置UTF-8支持
target_compile_options(CHTLContext PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)

# 创建CHTLParser静态库
set(CHTLPARSER_SOURCES
    CHTL/CHTLParser/CHTLParser.cpp
)

add_library(CHTLParser STATIC ${CHTLPARSER_SOURCES})

# 设置目标属性
set_target_properties(CHTLParser PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 创建CHTLJSLexer静态库
set(CHTLJSLEXER_SOURCES
    "CHTL JS/CHTLJSLexer/CHTLJSLexer.cpp"
)

add_library(CHTLJSLexer STATIC ${CHTLJSLEXER_SOURCES})

# 设置目标属性
set_target_properties(CHTLJSLexer PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 创建CHTLJSParser静态库
set(CHTLJSPARSER_SOURCES
    "CHTL JS/CHTLJSParser/CHTLJSParser.cpp"
)

add_library(CHTLJSParser STATIC ${CHTLJSPARSER_SOURCES})

# 设置目标属性
set_target_properties(CHTLJSParser PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接依赖库
target_link_libraries(CHTLJSParser PRIVATE CHTLJSLexer)

# 创建CHTLGenerator静态库
set(CHTLGENERATOR_SOURCES
    "CHTL/CHTLGenerator/CHTLGenerator.cpp"
)

add_library(CHTLGenerator STATIC ${CHTLGENERATOR_SOURCES})

# 设置目标属性
set_target_properties(CHTLGenerator PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 创建CHTLJSGenerator静态库
set(CHTLJSGENERATOR_SOURCES
    "CHTL JS/CHTLJSGenerator/CHTLJSGenerator.cpp"
)

add_library(CHTLJSGenerator STATIC ${CHTLJSGENERATOR_SOURCES})

# 设置目标属性
set_target_properties(CHTLJSGenerator PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

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