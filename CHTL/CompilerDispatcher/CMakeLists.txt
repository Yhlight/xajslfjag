cmake_minimum_required(VERSION 3.10)

add_library(compiler_dispatcher STATIC
    CompilerDispatcher.cpp
)

target_include_directories(compiler_dispatcher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(compiler_dispatcher scanner)