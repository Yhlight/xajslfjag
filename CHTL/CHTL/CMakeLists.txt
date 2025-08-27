cmake_minimum_required(VERSION 3.10)

# CHTL核心库
add_library(chtl_core STATIC
    CHTLContext.cpp
    CHTLGenerator.cpp
    CHTLLexer/Lexer.cpp
    CHTLLexer/Token.cpp
    CHTLLexer/GlobalMap.cpp
    CHTLLoader.cpp
    CHTLManage.cpp
    CHTLNode/BaseNode.cpp
    CHTLNode/CommentNode.cpp
    CHTLNode/TemplateNode.cpp
    CHTLNode/CustomNode.cpp
    CHTLNode/StyleNode.cpp
    CHTLNode/ScriptNode.cpp
    CHTLNode/OriginNode.cpp
    CHTLNode/ImportNode.cpp
    CHTLNode/ConfigNode.cpp
    CHTLNode/NamespaceNode.cpp
    CHTLNode/OperatorNode.cpp
    CHTLParser.cpp
    CHTLState.cpp
    CMODSystem.cpp
)

target_include_directories(chtl_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)