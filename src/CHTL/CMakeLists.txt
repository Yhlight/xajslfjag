# CHTL模块CMakeLists.txt

# 设置源文件
set(CHTL_SOURCES
    # CHTLLexer
    CHTLLexer/Token.cpp
    CHTLLexer/GlobalMap.cpp
    CHTLLexer/Lexer.cpp
    
    # CHTLState
    CHTLState/State.cpp
    
    # CHTLContext
    CHTLContext/Context.cpp
    
    # CHTLNode
    CHTLNode/BaseNode.cpp
    CHTLNode/StyleNode.cpp
    CHTLNode/ScriptNode.cpp
    CHTLNode/NodeVisitor.cpp
    
    # CHTLParser
    CHTLParser/Parser.cpp
    
    # CHTLGenerator
    CHTLGenerator/HtmlGenerator.cpp
)

# 设置头文件
set(CHTL_HEADERS
    # CHTLLexer
    CHTLLexer/Token.h
    CHTLLexer/GlobalMap.h
    CHTLLexer/Lexer.h
    
    # CHTLState
    CHTLState/State.h
    
    # CHTLContext
    CHTLContext/Context.h
    
    # CHTLNode
    CHTLNode/BaseNode.h
    CHTLNode/NodeVisitor.h
    CHTLNode/StyleNode.h
    CHTLNode/ScriptNode.h
    
    # CHTLParser
    CHTLParser/Parser.h
    
    # CHTLGenerator
    CHTLGenerator/HtmlGenerator.h
)

# 创建CHTL静态库
add_library(CHtml_CHTL STATIC ${CHTL_SOURCES} ${CHTL_HEADERS})

# 设置包含目录
target_include_directories(CHtml_CHTL PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTLGenerator
)

# 设置编译选项
target_compile_features(CHtml_CHTL PUBLIC cxx_std_17)

# UTF-8支持
if(MSVC)
    target_compile_options(CHtml_CHTL PRIVATE /utf-8)
endif()

# 链接ANTLR4库（如果需要）
target_link_libraries(CHtml_CHTL ${ANTLR4_LIBRARIES})

# 设置目标属性
set_target_properties(CHtml_CHTL PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)