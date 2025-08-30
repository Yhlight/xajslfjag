# 工具类CMake配置

add_subdirectory(FileSystem)
add_subdirectory(ZIPUtil)

# 创建工具类库
add_library(CHTLUtil STATIC
    # 工具类源文件会从子目录添加
)

# 链接子组件
target_link_libraries(CHTLUtil
    CHTLFileSystem
    CHTLZIPUtil
)

# 设置包含目录
target_include_directories(CHTLUtil PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)