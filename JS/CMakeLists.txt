# JS模块 - 使用ANTLR4

# 设置JS源文件
set(JS_SOURCES
    JavaScriptLexer.cpp
    JavaScriptParser.cpp
    JavaScriptParserBase.cpp
    JavaScriptParserBaseListener.cpp
    JavaScriptParserListener.cpp
)

# 创建JS静态库
add_library(JS STATIC ${JS_SOURCES})

# 设置目标属性
set_target_properties(JS PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接ANTLR4库
target_link_libraries(JS antlr4-runtime)