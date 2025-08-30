# 测试系统CMake配置

add_subdirectory(TokenTest)
add_subdirectory(ASTTest)  
add_subdirectory(CompilerTimeMonitor)

# 占位目录
add_library(UtilTest INTERFACE)

# 创建测试系统库
add_library(CHTLTest STATIC
    # 测试相关源文件
)

# 链接测试组件
target_link_libraries(CHTLTest
    CHTLTokenTest
    CHTLASTTest
    CHTLCompilerTimeMonitor
    UtilTest
)

# 设置包含目录
target_include_directories(CHTLTest PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)