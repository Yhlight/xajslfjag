# CHTLJSManage子模块

# 设置CHTLJSManage源文件
set(CHTLJSMANAGE_SOURCES
    # 这里将添加CHTLJSManage的源文件
)

# 创建CHTLJSManage静态库
add_library(CHTLJSManage STATIC ${CHTLJSMANAGE_SOURCES})

# 设置目标属性
set_target_properties(CHTLJSManage PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)