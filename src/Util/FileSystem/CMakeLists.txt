# FileSystem模块CMakeLists.txt

# 设置源文件
set(FILESYSTEM_SOURCES
    FileSystem.cpp
)

# 设置头文件
set(FILESYSTEM_HEADERS
    FileSystem.h
)

# 创建FileSystem静态库
add_library(CHtml_FileSystem STATIC ${FILESYSTEM_SOURCES} ${FILESYSTEM_HEADERS})

# 设置包含目录
target_include_directories(CHtml_FileSystem PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置编译选项
target_compile_features(CHtml_FileSystem PUBLIC cxx_std_17)

# UTF-8支持
if(MSVC)
    target_compile_options(CHtml_FileSystem PRIVATE /utf-8)
endif()

# 设置目标属性
set_target_properties(CHtml_FileSystem PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)