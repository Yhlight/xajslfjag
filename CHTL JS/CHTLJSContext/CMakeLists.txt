# CHTLJSContext子模块

# 设置CHTLJSContext源文件
set(CHTLJSCONTEXT_SOURCES
    # 这里将添加CHTLJSContext的源文件
)

# 创建CHTLJSContext静态库
add_library(CHTLJSContext STATIC ${CHTLJSCONTEXT_SOURCES})

# 设置目标属性
set_target_properties(CHTLJSContext PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)