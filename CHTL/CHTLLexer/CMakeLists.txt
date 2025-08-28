# CHTLLexer子模块

# 设置CHTLLexer源文件
set(CHTLLEXER_SOURCES
    CHTLToken.cpp
    CHTLGlobalMap.cpp
    CHTLLexer.cpp
)

# 创建CHTLLexer静态库
add_library(CHTLLexer STATIC ${CHTLLEXER_SOURCES})

# 设置目标属性
set_target_properties(CHTLLexer PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)