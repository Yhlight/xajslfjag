# CHTL JS编译器CMake配置文件

# CHTL JS核心组件
add_subdirectory(CHTLJSContext)
add_subdirectory(CHTLJSGenerator)
add_subdirectory(CHTLJSLexer)
add_subdirectory(CHTLJSLoader)
add_subdirectory(CHTLJSManage)
add_subdirectory(CHTLJSNode)
add_subdirectory(CHTLJSParser)
add_subdirectory(CHTLJSState)
add_subdirectory(CHTLJSIOStream)
add_subdirectory(CJMODSystem)

# 创建CHTL JS核心库
add_library(CHTLJSCore STATIC
    # 这里会自动包含所有子目录的源文件
)

# 链接所有CHTL JS组件
target_link_libraries(CHTLJSCore
    CHTLJSContext
    CHTLJSGenerator
    CHTLJSLexer
    CHTLJSLoader
    CHTLJSManage
    CHTLJSNode
    CHTLJSParser
    CHTLJSState
    CHTLJSIOStream
    CJMODSystem
    CHTLError
    CHTLUtil
)

# 设置包含目录
target_include_directories(CHTLJSCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)