# CMakeLists.txt for CHTL Test System
cmake_minimum_required(VERSION 3.16)

# 设置项目信息
project(CHTLTest VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 查找依赖
find_package(Threads REQUIRED)

# 设置源文件
set(CHTL_TEST_SOURCES
    # UtilTest
    UtilTest/NodeTest.cpp
    
    # TokenTestUtil (headers only for now)
    
    # ASTTestUtil (headers only for now)
    
    # CompilationMonitor
    CompilationMonitor/CompilationMonitor.cpp
)

set(CHTL_TEST_HEADERS
    # UtilTest
    UtilTest/NodeTest.h
    
    # TokenTestUtil
    TokenTestUtil/TokenPrint.h
    TokenTestUtil/TokenTable.h
    
    # ASTTestUtil
    ASTTestUtil/ASTPrint.h
    ASTTestUtil/ASTGraph.h
    
    # CompilationMonitor
    CompilationMonitor/CompilationMonitor.h
)

# 创建测试库
add_library(CHTLTest STATIC ${CHTL_TEST_SOURCES} ${CHTL_TEST_HEADERS})

# 设置包含目录
target_include_directories(CHTLTest PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# 链接依赖
target_link_libraries(CHTLTest PUBLIC
    CHTLError
    CHTLLexer
    CHTLNode
    Threads::Threads
)

# 设置目标属性
set_target_properties(CHTLTest PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${CHTL_TEST_HEADERS}"
)

# 创建测试可执行文件
add_executable(run_all_tests run_all_tests.cpp)
target_link_libraries(run_all_tests 
    CHTLTest
    CHTLError
    CHTLLexer 
    CHTLNode
)

# 创建编译监视器测试程序
add_executable(test_compilation_monitor test_compilation_monitor.cpp)
target_link_libraries(test_compilation_monitor 
    CHTLTest
    CHTLError
)

# 设置包含目录
target_include_directories(run_all_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Error
    ${CMAKE_CURRENT_SOURCE_DIR}/../CHTL/CHTLLexer
    ${CMAKE_CURRENT_SOURCE_DIR}/../CHTL/CHTLNode
)

target_include_directories(test_compilation_monitor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Error
)