# CSS编译器构建配置（基于ANTLR4）

# 源文件
set(CSS_SOURCES
    CSSCompiler.cpp
    CSSParser.cpp
    CSSGenerator.cpp
)

# 头文件
set(CSS_HEADERS
    CSSCompiler.h
    CSSParser.h
    CSSGenerator.h
)

# 创建CSS编译器库
add_library(css_compiler STATIC ${CSS_SOURCES} ${CSS_HEADERS})

# 设置目标属性
set_target_properties(css_compiler PROPERTIES
    OUTPUT_NAME "css_compiler"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 包含目录
target_include_directories(css_compiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/css
)

# 链接ANTLR4生成的CSS解析器
target_link_libraries(css_compiler
    ${CMAKE_SOURCE_DIR}/css/css3Parser
    ${CMAKE_SOURCE_DIR}/css/css3Lexer
)

# 安装规则
install(TARGETS css_compiler
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${CSS_HEADERS}
    DESTINATION include/css_compiler
)