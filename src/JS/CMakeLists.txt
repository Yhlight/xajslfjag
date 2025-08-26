add_library(JSCompiler STATIC
    JSCompiler.cpp
    ${CMAKE_SOURCE_DIR}/js/JavaScriptLexer.cpp
    ${CMAKE_SOURCE_DIR}/js/JavaScriptLexerBase.cpp
    ${CMAKE_SOURCE_DIR}/js/JavaScriptParser.cpp
    ${CMAKE_SOURCE_DIR}/js/JavaScriptParserBase.cpp
    ${CMAKE_SOURCE_DIR}/js/JavaScriptParserBaseListener.cpp
    ${CMAKE_SOURCE_DIR}/js/JavaScriptParserListener.cpp
)

target_include_directories(JSCompiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/js
    ${CMAKE_SOURCE_DIR}/ANTLR4/include/antlr4-runtime
)

target_link_libraries(JSCompiler PUBLIC
    CHTLCommon
    antlr4-runtime
)