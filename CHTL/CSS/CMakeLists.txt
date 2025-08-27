cmake_minimum_required(VERSION 3.10)

add_library(css_compiler STATIC
    CSSCompiler.cpp
)

target_include_directories(css_compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

# 如果需要ANTLR4
# find_package(antlr4-runtime REQUIRED)
# target_link_libraries(css_compiler antlr4-runtime)