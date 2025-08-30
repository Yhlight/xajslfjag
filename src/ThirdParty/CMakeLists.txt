# 第三方库CMake配置

# ========== ANTLR4运行时库配置 ==========

# 设置ANTLR4路径
set(ANTLR4_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/antlr)
set(ANTLR4_INCLUDE_DIR ${ANTLR4_ROOT}/ANTLR4/include)
set(ANTLR4_GENERATED_DIR ${ANTLR4_ROOT}/generated)
set(ANTLR4_LIB_DIR ${ANTLR4_ROOT}/ANTLR4/lib)

# 平台特定的库路径
if(WIN32)
    set(ANTLR4_PLATFORM_LIB_DIR ${ANTLR4_LIB_DIR}/windows)
    set(ANTLR4_STATIC_LIB ${ANTLR4_PLATFORM_LIB_DIR}/antlr4-runtime-static.lib)
    set(ANTLR4_DYNAMIC_LIB ${ANTLR4_PLATFORM_LIB_DIR}/antlr4-runtime.lib)
else()
    set(ANTLR4_PLATFORM_LIB_DIR ${ANTLR4_LIB_DIR}/linux)
    set(ANTLR4_STATIC_LIB ${ANTLR4_PLATFORM_LIB_DIR}/libantlr4-runtime.a)
endif()

# 验证库文件存在
if(NOT EXISTS ${ANTLR4_STATIC_LIB})
    message(FATAL_ERROR "ANTLR4静态库不存在: ${ANTLR4_STATIC_LIB}")
endif()

message(STATUS "ANTLR4静态库路径: ${ANTLR4_STATIC_LIB}")

# ========== 创建ANTLR4运行时库目标 ==========

# 创建导入库目标
add_library(ANTLR4Runtime STATIC IMPORTED)

# 设置库文件路径
set_target_properties(ANTLR4Runtime PROPERTIES
    IMPORTED_LOCATION ${ANTLR4_STATIC_LIB}
    INTERFACE_INCLUDE_DIRECTORIES "${ANTLR4_INCLUDE_DIR};${ANTLR4_INCLUDE_DIR}/atn;${ANTLR4_INCLUDE_DIR}/dfa;${ANTLR4_INCLUDE_DIR}/misc;${ANTLR4_INCLUDE_DIR}/support;${ANTLR4_INCLUDE_DIR}/tree;${ANTLR4_INCLUDE_DIR}/internal"
)

# 平台特定配置
if(WIN32)
    set_target_properties(ANTLR4Runtime PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS "ANTLR4CPP_STATIC;_CRT_SECURE_NO_WARNINGS"
        INTERFACE_LINK_LIBRARIES "ws2_32;wsock32"
    )
else()
    set_target_properties(ANTLR4Runtime PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS "ANTLR4CPP_STATIC"
        INTERFACE_LINK_LIBRARIES "pthread;dl"
    )
endif()

# ========== CSS解析器 ==========

# CSS生成的解析器源文件（使用更完整的css3解析器）
set(CSS_PARSER_SOURCES
    ${ANTLR4_GENERATED_DIR}/CSS/css3Lexer.cpp
    ${ANTLR4_GENERATED_DIR}/CSS/css3Parser.cpp
    ${ANTLR4_GENERATED_DIR}/CSS/css3ParserBaseListener.cpp
    ${ANTLR4_GENERATED_DIR}/CSS/css3ParserListener.cpp
)

set(CSS_PARSER_HEADERS
    ${ANTLR4_GENERATED_DIR}/CSS/css3Lexer.h
    ${ANTLR4_GENERATED_DIR}/CSS/css3Parser.h
    ${ANTLR4_GENERATED_DIR}/CSS/css3ParserBaseListener.h
    ${ANTLR4_GENERATED_DIR}/CSS/css3ParserListener.h
)

# 验证CSS解析器文件存在
foreach(CSS_FILE ${CSS_PARSER_SOURCES})
    if(NOT EXISTS ${CSS_FILE})
        message(WARNING "CSS解析器文件不存在: ${CSS_FILE}")
    endif()
endforeach()

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

# JavaScript生成的解析器源文件（使用更完整的JavaScript解析器）
set(JS_PARSER_SOURCES
    ${ANTLR4_GENERATED_DIR}/JS/JavaScriptLexer.cpp
    ${ANTLR4_GENERATED_DIR}/JS/JavaScriptParser.cpp
    ${ANTLR4_GENERATED_DIR}/JS/JavaScriptLexerBase.cpp
    ${ANTLR4_GENERATED_DIR}/JS/JavaScriptParserBase.cpp
    ${ANTLR4_GENERATED_DIR}/JS/JavaScriptParserBaseListener.cpp
    ${ANTLR4_GENERATED_DIR}/JS/JavaScriptParserListener.cpp
)

set(JS_PARSER_HEADERS
    ${ANTLR4_GENERATED_DIR}/JS/JavaScriptLexer.h
    ${ANTLR4_GENERATED_DIR}/JS/JavaScriptParser.h
    ${ANTLR4_GENERATED_DIR}/JS/JavaScriptLexerBase.h
    ${ANTLR4_GENERATED_DIR}/JS/JavaScriptParserBase.h
    ${ANTLR4_GENERATED_DIR}/JS/JavaScriptParserBaseListener.h
    ${ANTLR4_GENERATED_DIR}/JS/JavaScriptParserListener.h
)

# 验证JavaScript解析器文件存在
foreach(JS_FILE ${JS_PARSER_SOURCES})
    if(NOT EXISTS ${JS_FILE})
        message(WARNING "JavaScript解析器文件不存在: ${JS_FILE}")
    endif()
endforeach()

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

# ========== 导出变量供其他模块使用 ==========
set(ANTLR4_INCLUDE_DIRS ${ANTLR4_INCLUDE_DIR} PARENT_SCOPE)
set(ANTLR4_LIBRARIES ANTLR4Runtime ANTLRCSSParser ANTLRJSParser PARENT_SCOPE)
set(ANTLR4_FOUND TRUE PARENT_SCOPE)

# 输出配置信息
message(STATUS "ANTLR4配置完成:")
message(STATUS "  包含目录: ${ANTLR4_INCLUDE_DIR}")
message(STATUS "  库目录: ${ANTLR4_PLATFORM_LIB_DIR}")
message(STATUS "  静态库: ${ANTLR4_STATIC_LIB}")
if(WIN32)
    message(STATUS "  动态库: ${ANTLR4_DYNAMIC_LIB}")
endif()