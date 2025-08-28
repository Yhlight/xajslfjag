# FileSystem子模块

# 设置FileSystem源文件
set(FILESYSTEM_SOURCES
    FileSystem.cpp
    PathUtils.cpp
)

# 创建FileSystem静态库
add_library(FileSystem STATIC ${FILESYSTEM_SOURCES})

# 设置目标属性
set_target_properties(FileSystem PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)