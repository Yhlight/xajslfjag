cmake_minimum_required(VERSION 3.16)
project(CHTL_Standard_Structure LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 简化的测试构建
add_executable(test_standard_structure test_standard_structure.cpp)

# 基础测试（不依赖复杂库）
add_executable(structure_validation 
    test_standard_structure.cpp
)

# 最终验证程序
add_executable(final_verification 
    final_verification.cpp
)