# CHTLJSParser子模块

# 设置CHTLJSParser源文件
set(CHTLJSPARSER_SOURCES
    # 这里将添加CHTLJSParser的源文件
)

# 创建CHTLJSParser静态库
add_library(CHTLJSParser STATIC ${CHTLJSPARSER_SOURCES})

# 设置目标属性
set_target_properties(CHTLJSParser PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)