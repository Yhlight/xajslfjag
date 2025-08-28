# CHTLJSState子模块

# 设置CHTLJSState源文件
set(CHTLJSSTATE_SOURCES
    # 这里将添加CHTLJSState的源文件
)

# 创建CHTLJSState静态库
add_library(CHTLJSState STATIC ${CHTLJSSTATE_SOURCES})

# 设置目标属性
set_target_properties(CHTLJSState PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)