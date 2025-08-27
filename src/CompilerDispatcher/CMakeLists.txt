# CompilerDispatcher模块CMakeLists.txt

# 设置源文件
set(DISPATCHER_SOURCES
    CompilerDispatcher.cpp
)

# 设置头文件
set(DISPATCHER_HEADERS
    CompilerDispatcher.h
)

# 创建CompilerDispatcher静态库
add_library(CHtml_CompilerDispatcher STATIC ${DISPATCHER_SOURCES} ${DISPATCHER_HEADERS})

# 设置包含目录
target_include_directories(CHtml_CompilerDispatcher PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# 设置编译选项
target_compile_features(CHtml_CompilerDispatcher PUBLIC cxx_std_17)

# UTF-8支持
if(MSVC)
    target_compile_options(CHtml_CompilerDispatcher PRIVATE /utf-8)
endif()

# 链接依赖库
target_link_libraries(CHtml_CompilerDispatcher
    CHtml_Scanner
    CHtml_CHTL
    ${ANTLR4_LIBRARIES}
)

# 设置目标属性
set_target_properties(CHtml_CompilerDispatcher PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)