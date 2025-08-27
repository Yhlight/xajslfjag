# Util模块CMakeLists.txt

# 添加子目录
add_subdirectory(FileSystem)

# 创建Util静态库（接口库，因为它只是包装其他模块）
add_library(CHtml_Util INTERFACE)

# 链接子模块
target_link_libraries(CHtml_Util INTERFACE
    CHtml_FileSystem
)

# 设置包含目录
target_include_directories(CHtml_Util INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置编译选项
target_compile_features(CHtml_Util INTERFACE cxx_std_17)