# JavaScript编译器库

add_library(JS_Compiler STATIC
    JSCompiler.cpp
    JSCompiler.h
)

target_include_directories(JS_Compiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/antlr/ANTLR4/include
)

target_link_libraries(JS_Compiler
    ${ANTLR4_LIBRARIES}
    Error_System
)

target_compile_features(JS_Compiler PUBLIC cxx_std_17)

message(STATUS "JavaScript编译器库配置完成")