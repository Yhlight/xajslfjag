cmake_minimum_required(VERSION 3.16)
project(CHTL_Complete LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# 添加include目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# ========== 第三方库（ANTLR4） ==========
add_subdirectory(ThirdParty)

# ========== 错误处理系统 ==========
add_subdirectory(Error)

# ========== 工具类 ==========
add_subdirectory(Util)

# ========== 综合测试程序 ==========
add_executable(comprehensive_test comprehensive_test.cpp)
target_link_libraries(comprehensive_test 
    CHTLError
)

# 最终验证程序
add_executable(final_verification final_verification.cpp)
target_link_libraries(final_verification 
    CHTLError
)

# 结构验证程序
add_executable(structure_validation test_standard_structure.cpp)
target_link_libraries(structure_validation 
    CHTLError
)

# ANTLR4集成测试程序
add_executable(test_antlr_integration test_antlr_integration.cpp)
target_link_libraries(test_antlr_integration 
    CHTLError
)