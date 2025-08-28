# 通用工具库构建配置

# 源文件
set(UTIL_SOURCES
    FileSystem/FileSystem.cpp
    FileSystem/PathUtils.cpp
    FileSystem/FileUtils.cpp
    StringUtils.cpp
    ConfigManager.cpp
    Logger.cpp
)

# 头文件
set(UTIL_HEADERS
    FileSystem/FileSystem.h
    FileSystem/PathUtils.h
    FileSystem/FileUtils.h
    StringUtils.h
    ConfigManager.h
    Logger.h
)

# 创建工具库
add_library(util STATIC ${UTIL_SOURCES} ${UTIL_HEADERS})

# 设置目标属性
set_target_properties(util PROPERTIES
    OUTPUT_NAME "util"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 包含目录
target_include_directories(util PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# 安装规则
install(TARGETS util
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${UTIL_HEADERS}
    DESTINATION include/util
)