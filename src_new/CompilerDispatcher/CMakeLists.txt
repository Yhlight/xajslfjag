# 编译器调度器库

add_library(CompilerDispatcher_Core STATIC
    CompilerDispatcher.cpp
    CompilerDispatcher.h
)

target_include_directories(CompilerDispatcher_Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(CompilerDispatcher_Core
    CHTL_Core
    CHTL_JS_Core
    Scanner_Core
    CSS_Compiler
    JS_Compiler
    Error_System
)

target_compile_features(CompilerDispatcher_Core PUBLIC cxx_std_17)

message(STATUS "编译器调度器库配置完成")