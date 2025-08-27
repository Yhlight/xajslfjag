# Scanner模块CMakeLists.txt

# 设置源文件
set(SCANNER_SOURCES
    CHTLUnifiedScanner.cpp
)

# 设置头文件
set(SCANNER_HEADERS
    CHTLUnifiedScanner.h
)

# 创建Scanner静态库
add_library(CHtml_Scanner STATIC ${SCANNER_SOURCES} ${SCANNER_HEADERS})

# 设置包含目录
target_include_directories(CHtml_Scanner PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置编译选项
target_compile_features(CHtml_Scanner PUBLIC cxx_std_17)

# UTF-8支持
if(MSVC)
    target_compile_options(CHtml_Scanner PRIVATE /utf-8)
endif()

# 设置目标属性
set_target_properties(CHtml_Scanner PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)