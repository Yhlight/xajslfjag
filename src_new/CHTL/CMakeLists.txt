# CHTL编译器CMake配置文件

# CHTL核心组件
add_subdirectory(CHTLContext)
add_subdirectory(CHTLGenerator)
add_subdirectory(CHTLLexer)
add_subdirectory(CHTLLoader)
add_subdirectory(CHTLManage)
add_subdirectory(CHTLNode)
add_subdirectory(CHTLParser)
add_subdirectory(CHTLState)
add_subdirectory(CHTLIOStream)
add_subdirectory(CMODSystem)

# 创建CHTL核心库
add_library(CHTLCore STATIC
    # 这里会自动包含所有子目录的源文件
)

# 链接所有CHTL组件
target_link_libraries(CHTLCore
    CHTLContext
    CHTLGenerator
    CHTLLexer
    CHTLLoader
    CHTLManage
    CHTLNode
    CHTLParser
    CHTLState
    CHTLIOStream
    CMODSystem
    CHTLError
    CHTLUtil
)

# 设置包含目录
target_include_directories(CHTLCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)