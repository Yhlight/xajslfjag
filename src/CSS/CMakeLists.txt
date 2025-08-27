# CSS模块CMakeLists.txt

# 将现有的CSS ANTLR文件复制到构建目录
file(COPY ${CMAKE_SOURCE_DIR}/css/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

# 设置CSS编译器源文件（基于ANTLR生成的文件）
set(CSS_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/css3Lexer.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/css3Parser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/css3ParserBaseListener.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/css3ParserListener.cpp
)

# 设置CSS编译器头文件
set(CSS_HEADERS
    ${CMAKE_CURRENT_BINARY_DIR}/css3Lexer.h
    ${CMAKE_CURRENT_BINARY_DIR}/css3Parser.h
    ${CMAKE_CURRENT_BINARY_DIR}/css3ParserBaseListener.h
    ${CMAKE_CURRENT_BINARY_DIR}/css3ParserListener.h
)

# 创建CSS静态库
add_library(CHtml_CSS STATIC ${CSS_SOURCES} ${CSS_HEADERS})

# 设置包含目录
target_include_directories(CHtml_CSS PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${ANTLR4_INCLUDE_DIR}
)

# 设置编译选项
target_compile_features(CHtml_CSS PUBLIC cxx_std_17)

# UTF-8支持
if(MSVC)
    target_compile_options(CHtml_CSS PRIVATE /utf-8)
endif()

# 链接ANTLR4库
target_link_libraries(CHtml_CSS ${ANTLR4_LIBRARIES})

# 设置目标属性
set_target_properties(CHtml_CSS PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)