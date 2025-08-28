# CHTL JS模块

# 添加子目录
add_subdirectory(CHTLJSContext)
add_subdirectory(CHTLJSGenerator)
add_subdirectory(CHTLJSLexer)
add_subdirectory(CHTLJSLoader)
add_subdirectory(CHTLJSManage)
add_subdirectory(CHTLJSNode)
add_subdirectory(CHTLJSParser)
add_subdirectory(CHTLJSState)
add_subdirectory(CJMODSystem)

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