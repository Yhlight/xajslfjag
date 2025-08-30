# CSS编译器库

add_library(CSS_Compiler STATIC
    CSSCompiler.cpp
    CSSCompiler.h
)

target_include_directories(CSS_Compiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/antlr/ANTLR4/include
)

target_link_libraries(CSS_Compiler
    ${ANTLR4_LIBRARIES}
    Error_System
)

target_compile_features(CSS_Compiler PUBLIC cxx_std_17)

message(STATUS "CSS编译器库配置完成")