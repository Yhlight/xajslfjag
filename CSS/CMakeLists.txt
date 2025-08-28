# CSS模块 - 使用ANTLR4

# 设置CSS源文件
set(CSS_SOURCES
    css3Lexer.cpp
    css3Parser.cpp
    css3ParserBaseListener.cpp
    css3ParserListener.cpp
)

# 创建CSS静态库
add_library(CSS STATIC ${CSS_SOURCES})

# 设置目标属性
set_target_properties(CSS PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接ANTLR4库
target_link_libraries(CSS antlr4-runtime)