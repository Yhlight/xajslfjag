add_library(CSSCompiler STATIC
    CSSCompiler.cpp
    ${CMAKE_SOURCE_DIR}/css/css3Lexer.cpp
    ${CMAKE_SOURCE_DIR}/css/css3Parser.cpp
    ${CMAKE_SOURCE_DIR}/css/css3ParserBaseListener.cpp
    ${CMAKE_SOURCE_DIR}/css/css3ParserListener.cpp
)

target_include_directories(CSSCompiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/css
    ${CMAKE_SOURCE_DIR}/ANTLR4/include/antlr4-runtime
)

target_link_libraries(CSSCompiler PUBLIC
    CHTLCommon
    antlr4-runtime
)