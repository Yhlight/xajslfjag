# CMakeLists.txt for CHTL Project
cmake_minimum_required(VERSION 3.16)

# 设置项目信息
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 添加子目录
add_subdirectory(../Error CHTLError)
add_subdirectory(CHTLLexer)
add_subdirectory(CHTLState)
add_subdirectory(CHTLContext)
add_subdirectory(CHTLNode)
add_subdirectory(../Test CHTLTest)

# 如果需要构建测试程序
option(BUILD_TESTS "Build test programs" ON)

if(BUILD_TESTS)
    # 创建综合测试程序
    add_executable(test_state_context test_state_context.cpp)
    target_link_libraries(test_state_context 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLTest
    )
    
    # 创建更新后的lexer测试程序
    add_executable(test_updated_lexer test_updated_lexer.cpp)
    target_link_libraries(test_updated_lexer 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
    )
    
    # 设置包含目录
    target_include_directories(test_state_context PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/../Test
    )
    
    target_include_directories(test_updated_lexer PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/../Test
    )
endif()