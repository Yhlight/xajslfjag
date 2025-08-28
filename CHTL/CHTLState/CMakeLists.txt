# CHTLState子模块

# 设置CHTLState源文件
set(CHTLSTATE_SOURCES
    # 这里将添加CHTLState的源文件
)

# 创建CHTLState静态库
add_library(CHTLState STATIC ${CHTLSTATE_SOURCES})

# 设置目标属性
set_target_properties(CHTLState PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)