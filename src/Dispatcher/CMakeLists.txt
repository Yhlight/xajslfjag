add_library(CHTLDispatcher STATIC
    CompilerDispatcher.cpp
)

target_link_libraries(CHTLDispatcher PUBLIC
    CHTLCommon
    CHTLScanner
    CHTLCompiler
    CHTLJSCompiler
    CSSCompiler
    JSCompiler
)

target_include_directories(CHTLDispatcher PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)