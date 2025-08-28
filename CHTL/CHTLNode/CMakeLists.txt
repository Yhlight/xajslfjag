# CHTLNode子模块

# 设置CHTLNode源文件
set(CHTLNODE_SOURCES
    # 这里将添加CHTLNode的源文件
)

# 创建CHTLNode静态库
add_library(CHTLNode STATIC ${CHTLNODE_SOURCES})

# 设置目标属性
set_target_properties(CHTLNode PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)