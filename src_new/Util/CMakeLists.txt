# 工具类CMake配置

add_subdirectory(FileSystem)
add_subdirectory(ZIPUtil)

# 创建工具类库
add_library(CHTLUtil INTERFACE)

# 链接子组件
target_link_libraries(CHTLUtil INTERFACE
    CHTLFileSystem
    CHTLZIPUtil
)

# 设置包含目录
target_include_directories(CHTLUtil INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)