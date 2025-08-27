# JS模块CMakeLists.txt

# 将现有的JS ANTLR文件复制到构建目录
file(COPY ${CMAKE_SOURCE_DIR}/js/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

# 设置JavaScript编译器源文件（基于ANTLR生成的文件）
set(JS_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/JavaScriptLexer.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/JavaScriptLexerBase.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/JavaScriptParser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/JavaScriptParserBase.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/JavaScriptParserBaseListener.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/JavaScriptParserListener.cpp
)

# 设置JavaScript编译器头文件
set(JS_HEADERS
    ${CMAKE_CURRENT_BINARY_DIR}/JavaScriptLexer.h
    ${CMAKE_CURRENT_BINARY_DIR}/JavaScriptLexerBase.h
    ${CMAKE_CURRENT_BINARY_DIR}/JavaScriptParser.h
    ${CMAKE_CURRENT_BINARY_DIR}/JavaScriptParserBase.h
    ${CMAKE_CURRENT_BINARY_DIR}/JavaScriptParserBaseListener.h
    ${CMAKE_CURRENT_BINARY_DIR}/JavaScriptParserListener.h
)

# 创建JS静态库
add_library(CHtml_JS STATIC ${JS_SOURCES} ${JS_HEADERS})

# 设置包含目录
target_include_directories(CHtml_JS PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${ANTLR4_INCLUDE_DIR}
)

# 设置编译选项
target_compile_features(CHtml_JS PUBLIC cxx_std_17)

# UTF-8支持
if(MSVC)
    target_compile_options(CHtml_JS PRIVATE /utf-8)
endif()

# 链接ANTLR4库
target_link_libraries(CHtml_JS ${ANTLR4_LIBRARIES})

# 设置目标属性
set_target_properties(CHtml_JS PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)