cmake_minimum_required(VERSION 3.10)

add_library(chtljs_core STATIC
    CHTLJSContext.cpp
    CHTLJSGenerator.cpp
    CHTLJSLexer/Lexer.cpp
    CHTLJSLexer/Token.cpp
    CHTLJSLexer/GlobalMap.cpp
    CHTLJSLoader.cpp
    CHTLJSManage.cpp
    CHTLJSNode.cpp
    CHTLJSParser.cpp
    CHTLJSState.cpp
    CJMODSystem.cpp
)

target_include_directories(chtljs_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)