# CHTLContext子模块

# 设置CHTLContext源文件
set(CHTLCONTEXT_SOURCES
    # 这里将添加CHTLContext的源文件
)

# 创建CHTLContext静态库
add_library(CHTLContext STATIC ${CHTLCONTEXT_SOURCES})

# 设置目标属性
set_target_properties(CHTLContext PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)