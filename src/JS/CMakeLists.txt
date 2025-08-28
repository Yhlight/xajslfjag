# JavaScript编译器构建配置（基于ANTLR4）

# 源文件
set(JS_SOURCES
    JSCompiler.cpp
    JSParser.cpp
    JSGenerator.cpp
)

# 头文件
set(JS_HEADERS
    JSCompiler.h
    JSParser.h
    JSGenerator.h
)

# 创建JavaScript编译器库
add_library(js_compiler STATIC ${JS_SOURCES} ${JS_HEADERS})

# 设置目标属性
set_target_properties(js_compiler PROPERTIES
    OUTPUT_NAME "js_compiler"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 包含目录
target_include_directories(js_compiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/js
)

# 链接ANTLR4生成的JavaScript解析器
target_link_libraries(js_compiler
    ${CMAKE_SOURCE_DIR}/js/JavaScriptParser
    ${CMAKE_SOURCE_DIR}/js/JavaScriptLexer
)

# 安装规则
install(TARGETS js_compiler
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${JS_HEADERS}
    DESTINATION include/js_compiler
)