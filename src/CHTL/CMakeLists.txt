add_library(CHTLCompiler STATIC
    Token/CHTLToken.cpp
    Lexer/CHTLLexer.cpp
    Parser/CHTLParser.cpp
    AST/CHTLASTNode.cpp
    AST/ElementNode.cpp
    AST/TemplateNode.cpp
    AST/CustomNode.cpp
    AST/ConfigurationNode.cpp
    AST/ImportNode.cpp
    AST/NamespaceNode.cpp
    GlobalMap/CHTLGlobalMap.cpp
    Context/CHTLContext.cpp
    State/CHTLState.cpp
    Generator/CHTLGenerator.cpp
)

target_link_libraries(CHTLCompiler PUBLIC
    CHTLCommon
    CHTLScanner
)

target_include_directories(CHTLCompiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)