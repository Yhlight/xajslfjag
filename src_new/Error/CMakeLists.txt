# 错误处理系统库

add_library(Error_System STATIC
    ErrorReporter.cpp
    ErrorReporter.h
)

target_include_directories(Error_System PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(Error_System PUBLIC cxx_std_17)

message(STATUS "错误处理系统库配置完成")