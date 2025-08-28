# CHTLManage子模块

# 设置CHTLManage源文件
set(CHTLMANAGE_SOURCES
    # 这里将添加CHTLManage的源文件
)

# 创建CHTLManage静态库
add_library(CHTLManage STATIC ${CHTLMANAGE_SOURCES})

# 设置目标属性
set_target_properties(CHTLManage PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)