# 第三方库CMake配置

# ========== ANTLR4运行时库 ==========

# 设置ANTLR4路径
set(ANTLR4_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/antlr)
set(ANTLR4_INCLUDE_DIR ${ANTLR4_ROOT}/ANTLR4/include)
set(ANTLR4_GENERATED_DIR ${ANTLR4_ROOT}/generated)

# 收集ANTLR4运行时源文件
file(GLOB_RECURSE ANTLR4_RUNTIME_SOURCES 
    ${ANTLR4_INCLUDE_DIR}/*.cpp
    ${ANTLR4_INCLUDE_DIR}/*/*.cpp
    ${ANTLR4_INCLUDE_DIR}/*/*/*.cpp
)

# 收集ANTLR4运行时头文件
file(GLOB_RECURSE ANTLR4_RUNTIME_HEADERS 
    ${ANTLR4_INCLUDE_DIR}/*.h
    ${ANTLR4_INCLUDE_DIR}/*/*.h
    ${ANTLR4_INCLUDE_DIR}/*/*/*.h
)

# 创建ANTLR4运行时库
add_library(ANTLR4Runtime STATIC
    ${ANTLR4_RUNTIME_SOURCES}
    ${ANTLR4_RUNTIME_HEADERS}
)

# 设置ANTLR4包含目录
target_include_directories(ANTLR4Runtime PUBLIC
    ${ANTLR4_INCLUDE_DIR}
    ${ANTLR4_INCLUDE_DIR}/atn
    ${ANTLR4_INCLUDE_DIR}/dfa
    ${ANTLR4_INCLUDE_DIR}/misc
    ${ANTLR4_INCLUDE_DIR}/support
    ${ANTLR4_INCLUDE_DIR}/tree
    ${ANTLR4_INCLUDE_DIR}/internal
)

# ========== CSS解析器 ==========

# CSS生成的解析器源文件
set(CSS_PARSER_SOURCES
    ${ANTLR4_GENERATED_DIR}/CSS/CSSLexer.cpp
    ${ANTLR4_GENERATED_DIR}/CSS/CSSParser.cpp
    ${ANTLR4_GENERATED_DIR}/CSS/CSSParserBaseListener.cpp
    ${ANTLR4_GENERATED_DIR}/CSS/CSSParserListener.cpp
)

set(CSS_PARSER_HEADERS
    ${ANTLR4_GENERATED_DIR}/CSS/CSSLexer.h
    ${ANTLR4_GENERATED_DIR}/CSS/CSSParser.h
    ${ANTLR4_GENERATED_DIR}/CSS/CSSParserBaseListener.h
    ${ANTLR4_GENERATED_DIR}/CSS/CSSParserListener.h
)

# 创建CSS解析器库
add_library(ANTLRCSSParser STATIC
    ${CSS_PARSER_SOURCES}
    ${CSS_PARSER_HEADERS}
)

target_link_libraries(ANTLRCSSParser
    ANTLR4Runtime
)

target_include_directories(ANTLRCSSParser PUBLIC
    ${ANTLR4_GENERATED_DIR}/CSS
    ${ANTLR4_INCLUDE_DIR}
)

# ========== JavaScript解析器 ==========

# JavaScript生成的解析器源文件
set(JS_PARSER_SOURCES
    ${ANTLR4_GENERATED_DIR}/JS/SimpleJSLexer.cpp
    ${ANTLR4_GENERATED_DIR}/JS/SimpleJSParser.cpp
    ${ANTLR4_GENERATED_DIR}/JS/SimpleJSParserBaseListener.cpp
    ${ANTLR4_GENERATED_DIR}/JS/SimpleJSParserListener.cpp
)

set(JS_PARSER_HEADERS
    ${ANTLR4_GENERATED_DIR}/JS/SimpleJSLexer.h
    ${ANTLR4_GENERATED_DIR}/JS/SimpleJSParser.h
    ${ANTLR4_GENERATED_DIR}/JS/SimpleJSParserBaseListener.h
    ${ANTLR4_GENERATED_DIR}/JS/SimpleJSParserListener.h
)

# 创建JavaScript解析器库
add_library(ANTLRJSParser STATIC
    ${JS_PARSER_SOURCES}
    ${JS_PARSER_HEADERS}
)

target_link_libraries(ANTLRJSParser
    ANTLR4Runtime
)

target_include_directories(ANTLRJSParser PUBLIC
    ${ANTLR4_GENERATED_DIR}/JS
    ${ANTLR4_INCLUDE_DIR}
)

# ========== 平台特定配置 ==========

# Windows特定配置
if(WIN32)
    target_compile_definitions(ANTLR4Runtime PRIVATE
        ANTLR4CPP_STATIC
        _CRT_SECURE_NO_WARNINGS
    )
    
    # Windows特定链接库
    target_link_libraries(ANTLR4Runtime
        ws2_32
        wsock32
    )
endif()

# Linux特定配置
if(UNIX AND NOT APPLE)
    target_compile_definitions(ANTLR4Runtime PRIVATE
        ANTLR4CPP_STATIC
    )
    
    # Linux特定链接库
    target_link_libraries(ANTLR4Runtime
        pthread
        dl
    )
endif()

# 设置编译选项
target_compile_features(ANTLR4Runtime PUBLIC cxx_std_17)

# 导出ANTLR4变量供其他模块使用
set(ANTLR4_INCLUDE_DIRS ${ANTLR4_INCLUDE_DIR} PARENT_SCOPE)
set(ANTLR4_LIBRARIES ANTLR4Runtime ANTLRCSSParser ANTLRJSParser PARENT_SCOPE)