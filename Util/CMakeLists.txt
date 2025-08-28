# Util模块 - 工具函数

# 添加子目录
add_subdirectory(FileSystem)

# 设置Util源文件
set(UTIL_SOURCES
    StringUtils.cpp
    FileUtils.cpp
    LogUtils.cpp
)

# 创建Util静态库
add_library(Util STATIC ${UTIL_SOURCES})

# 设置目标属性
set_target_properties(Util PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接依赖库
target_link_libraries(Util
    FileSystem
)