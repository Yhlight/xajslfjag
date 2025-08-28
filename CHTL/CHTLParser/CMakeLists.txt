# CHTLParser子模块

# 设置CHTLParser源文件
set(CHTLPARSER_SOURCES
    # 这里将添加CHTLParser的源文件
)

# 创建CHTLParser静态库
add_library(CHTLParser STATIC ${CHTLPARSER_SOURCES})

# 设置目标属性
set_target_properties(CHTLParser PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)