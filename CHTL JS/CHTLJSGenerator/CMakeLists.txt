# CHTLJSGenerator子模块

# 设置CHTLJSGenerator源文件
set(CHTLJSGENERATOR_SOURCES
    # 这里将添加CHTLJSGenerator的源文件
)

# 创建CHTLJSGenerator静态库
add_library(CHTLJSGenerator STATIC ${CHTLJSGENERATOR_SOURCES})

# 设置目标属性
set_target_properties(CHTLJSGenerator PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)