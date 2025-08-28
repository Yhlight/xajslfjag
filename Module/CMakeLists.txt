# Module模块 - 模块管理

# 设置Module源文件
set(MODULE_SOURCES
    ModuleManager.cpp
    ModuleLoader.cpp
    ModuleRegistry.cpp
)

# 创建Module静态库
add_library(Module STATIC ${MODULE_SOURCES})

# 设置目标属性
set_target_properties(Module PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接依赖库
target_link_libraries(Module
    Util
    FileSystem
)