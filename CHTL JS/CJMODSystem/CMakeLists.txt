# CJMODSystem子模块

# 设置CJMODSystem源文件
set(CJMODSYSTEM_SOURCES
    # 这里将添加CJMODSystem的源文件
)

# 创建CJMODSystem静态库
add_library(CJMODSystem STATIC ${CJMODSYSTEM_SOURCES})

# 设置目标属性
set_target_properties(CJMODSystem PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)