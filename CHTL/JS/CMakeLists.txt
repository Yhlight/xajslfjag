cmake_minimum_required(VERSION 3.10)

add_library(js_compiler STATIC
    JSCompiler.cpp
)

target_include_directories(js_compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

# 如果需要ANTLR4
# find_package(antlr4-runtime REQUIRED)
# target_link_libraries(js_compiler antlr4-runtime)