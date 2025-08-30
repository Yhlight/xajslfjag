# 统一扫描器库

add_library(Scanner_Core STATIC
    UnifiedScanner.cpp
    UnifiedScanner.h
)

target_include_directories(Scanner_Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(Scanner_Core
    Error_System
)

target_compile_features(Scanner_Core PUBLIC cxx_std_17)

message(STATUS "统一扫描器库配置完成")