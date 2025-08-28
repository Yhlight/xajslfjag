# CHTL核心模块

# 添加子目录
add_subdirectory(CHTLContext)
add_subdirectory(CHTLGenerator)
add_subdirectory(CHTLLexer)
add_subdirectory(CHTLLoader)
add_subdirectory(CHTLManage)
add_subdirectory(CHTLNode)
add_subdirectory(CHTLParser)
add_subdirectory(CHTLState)
add_subdirectory(CMODSystem)

# 创建CHTL静态库
set(CHTL_SOURCES
    # 这里将添加CHTL模块的源文件
)

add_library(CHTL STATIC ${CHTL_SOURCES})

# 设置目标属性
set_target_properties(CHTL PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接依赖库
target_link_libraries(CHTL
    CHTLContext
    CHTLGenerator
    CHTLLexer
    CHTLLoader
    CHTLManage
    CHTLNode
    CHTLParser
    CHTLState
    CMODSystem
)