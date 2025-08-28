# CMODSystem子模块

# 设置CMODSystem源文件
set(CMODSYSTEM_SOURCES
    # 这里将添加CMODSystem的源文件
)

# 创建CMODSystem静态库
add_library(CMODSystem STATIC ${CMODSYSTEM_SOURCES})

# 设置目标属性
set_target_properties(CMODSystem PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)