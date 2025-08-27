# CHTL JS模块CMakeLists.txt

# 设置源文件
set(CHTLJS_SOURCES
    # CHTLJSLexer
    CHTLJSLexer/Token.cpp
    CHTLJSLexer/GlobalMap.cpp
)

# 设置头文件
set(CHTLJS_HEADERS
    # CHTLJSLexer
    CHTLJSLexer/Token.h
    CHTLJSLexer/GlobalMap.h
)

# 创建CHTL JS静态库
add_library(CHtml_CHTLJS STATIC ${CHTLJS_SOURCES} ${CHTLJS_HEADERS})

# 设置包含目录
target_include_directories(CHtml_CHTLJS PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTLJSLexer
)

# 设置编译选项
target_compile_features(CHtml_CHTLJS PUBLIC cxx_std_17)

# UTF-8支持
if(MSVC)
    target_compile_options(CHtml_CHTLJS PRIVATE /utf-8)
endif()

# 设置目标属性
set_target_properties(CHtml_CHTLJS PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)