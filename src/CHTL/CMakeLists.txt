# CHTL编译器构建配置

# 源文件
set(CHTL_SOURCES
    CHTLContext/CHTLContext.cpp
    CHTLGenerator/CHTLGenerator.cpp
    CHTLLexer/CHTLLexer.cpp
    CHTLLexer/GlobalMap.cpp
    CHTLLexer/Token.cpp
    CHTLLoader/CHTLLoader.cpp
    CHTLManage/CHTLManage.cpp
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
    CHTLParser/CHTLParser.cpp
    CHTLState/CHTLState.cpp
    CMODSystem/CMODSystem.cpp
)

# 头文件
set(CHTL_HEADERS
    CHTLContext/CHTLContext.h
    CHTLGenerator/CHTLGenerator.h
    CHTLLexer/CHTLLexer.h
    CHTLLexer/GlobalMap.h
    CHTLLexer/Token.h
    CHTLLoader/CHTLLoader.h
    CHTLManage/CHTLManage.h
    CHTLNode/BaseNode.h
    CHTLNode/CommentNode.h
    CHTLNode/TemplateNode.h
    CHTLNode/CustomNode.h
    CHTLNode/StyleNode.h
    CHTLNode/ScriptNode.h
    CHTLNode/OriginNode.h
    CHTLNode/ImportNode.h
    CHTLNode/ConfigNode.h
    CHTLNode/NamespaceNode.h
    CHTLNode/OperatorNode.h
    CHTLParser/CHTLParser.h
    CHTLState/CHTLState.h
    CMODSystem/CMODSystem.h
)

# 创建CHTL核心库
add_library(chtl_core STATIC ${CHTL_SOURCES} ${CHTL_HEADERS})

# 设置目标属性
set_target_properties(chtl_core PROPERTIES
    OUTPUT_NAME "chtl_core"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 包含目录
target_include_directories(chtl_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# 安装规则
install(TARGETS chtl_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${CHTL_HEADERS}
    DESTINATION include/chtl
)