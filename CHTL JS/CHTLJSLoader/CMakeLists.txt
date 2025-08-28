# CHTLJSLoader子模块

# 设置CHTLJSLoader源文件
set(CHTLJSLOADER_SOURCES
    # 这里将添加CHTLJSLoader的源文件
)

# 创建CHTLJSLoader静态库
add_library(CHTLJSLoader STATIC ${CHTLJSLOADER_SOURCES})

# 设置目标属性
set_target_properties(CHTLJSLoader PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)