# CHTLJSNode子模块

# 设置CHTLJSNode源文件
set(CHTLJSNODE_SOURCES
    # 这里将添加CHTLJSNode的源文件
)

# 创建CHTLJSNode静态库
add_library(CHTLJSNode STATIC ${CHTLJSNODE_SOURCES})

# 设置目标属性
set_target_properties(CHTLJSNode PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)