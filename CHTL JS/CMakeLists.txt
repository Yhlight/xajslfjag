# CHTL JS模块
# 这是一个容器模块，包含CHTL JS的所有子模块

# 添加子模块
add_subdirectory(CHTLJSNode)
add_subdirectory(CHTLJSParser)

# 创建CHTLJSParser静态库
set(CHTLJSPARSER_SOURCES
    CHTLJSParser/CHTLJSParser.cpp
)

add_library(CHTLJSParser STATIC ${CHTLJSPARSER_SOURCES})

# 设置目标属性
set_target_properties(CHTLJSParser PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 创建CHTLJS静态库
set(CHTLJS_SOURCES
    # 这里将添加CHTL JS模块的源文件
)

add_library(CHTLJS STATIC ${CHTLJS_SOURCES})

# 设置目标属性
set_target_properties(CHTLJS PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接依赖库
target_link_libraries(CHTLJS
    CHTLJSContext
    CHTLJSGenerator
    CHTLJSLexer
    CHTLJSLoader
    CHTLJSManage
    CHTLJSNode
    CHTLJSParser
    CHTLJSState
    CJMODSystem
)