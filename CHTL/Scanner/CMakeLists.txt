cmake_minimum_required(VERSION 3.10)

add_library(scanner STATIC
    Scanner.cpp
)

target_include_directories(scanner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)