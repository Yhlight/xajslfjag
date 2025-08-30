cmake_minimum_required(VERSION 3.16)
project(CHTL_ANTLR_Test LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# 添加include目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# ========== 第三方库（ANTLR4） ==========
add_subdirectory(ThirdParty)

# ========== ANTLR4库验证测试 ==========
add_executable(test_antlr_libraries test_antlr_libraries.cpp)
target_link_libraries(test_antlr_libraries)

message(STATUS "ANTLR4测试项目配置完成")