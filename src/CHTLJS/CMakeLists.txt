add_library(CHTLJSCompiler STATIC
    Token/CHTLJSToken.cpp
    Lexer/CHTLJSLexer.cpp
    Parser/CHTLJSParser.cpp
    AST/CHTLJSASTNode.cpp
    AST/EnhancedSelectorNode.cpp
    AST/VirtualObjectNode.cpp
    AST/EventNode.cpp
    AST/AnimationNode.cpp
    GlobalMap/CHTLJSGlobalMap.cpp
    Context/CHTLJSContext.cpp
    State/CHTLJSState.cpp
    Generator/CHTLJSGenerator.cpp
)

target_link_libraries(CHTLJSCompiler PUBLIC
    CHTLCommon
    CHTLScanner
)

target_include_directories(CHTLJSCompiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)