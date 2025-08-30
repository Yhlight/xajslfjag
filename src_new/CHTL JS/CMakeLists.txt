# CHTL JS编译器CMake配置文件

# CHTL JS核心组件（按照依赖顺序）
add_subdirectory(CHTLJSState)
add_subdirectory(CHTLJSLexer)
add_subdirectory(CHTLJSContext)
add_subdirectory(CHTLJSNode)
add_subdirectory(CHTLJSParser)
add_subdirectory(CHTLJSGenerator)
add_subdirectory(CJMODSystem)

# 占位目录（暂时为空库）
add_library(CHTLJSLoader INTERFACE)
add_library(CHTLJSManage INTERFACE)
add_library(CHTLJSIOStream INTERFACE)

# 创建CHTL JS核心库
add_library(CHTLJSCore STATIC
    # 这里会自动包含所有子目录的源文件
)

# 链接所有CHTL JS组件
target_link_libraries(CHTLJSCore
    CHTLJSState
    CHTLJSLexer
    CHTLJSContext
    CHTLJSNode
    CHTLJSParser
    CHTLJSGenerator
    CJMODSystem
    CHTLJSLoader
    CHTLJSManage
    CHTLJSIOStream
    CHTLError
)

# 设置包含目录
target_include_directories(CHTLJSCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)