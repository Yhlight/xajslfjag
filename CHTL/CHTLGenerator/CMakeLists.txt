# CHTLGenerator子模块

# 设置CHTLGenerator源文件
set(CHTLGENERATOR_SOURCES
    # 这里将添加CHTLGenerator的源文件
)

# 创建CHTLGenerator静态库
add_library(CHTLGenerator STATIC ${CHTLGENERATOR_SOURCES})

# 设置目标属性
set_target_properties(CHTLGenerator PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)