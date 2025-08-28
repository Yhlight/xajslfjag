# CHTLLoader子模块

# 设置CHTLLoader源文件
set(CHTLLOADER_SOURCES
    # 这里将添加CHTLLoader的源文件
)

# 创建CHTLLoader静态库
add_library(CHTLLoader STATIC ${CHTLLOADER_SOURCES})

# 设置目标属性
set_target_properties(CHTLLoader PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)