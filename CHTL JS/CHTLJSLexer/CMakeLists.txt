# CHTLJSLexer子模块

# 设置CHTLJSLexer源文件
set(CHTLJSLEXER_SOURCES
    CHTLJSToken.cpp
    CHTLJSLexer.cpp
)

# 创建CHTLJSLexer静态库
add_library(CHTLJSLexer STATIC ${CHTLJSLEXER_SOURCES})

# 设置目标属性
set_target_properties(CHTLJSLexer PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)