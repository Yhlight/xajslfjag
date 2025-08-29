# CMakeLists.txt for CHTL Node
cmake_minimum_required(VERSION 3.16)

# 设置项目信息
project(CHTLNode VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置源文件
set(CHTL_NODE_SOURCES
    BaseNode.cpp
    ElementNode.cpp
    TextNode.cpp
    CommentNode.cpp
    StyleNode.cpp
    TemplateNode.cpp
    AdvancedTemplateNode.cpp
    OperatorNode.cpp
    OriginNode.cpp
)

set(CHTL_NODE_HEADERS
    BaseNode.h
    ElementNode.h
    TextNode.h
    CommentNode.h
    StyleNode.h
    TemplateNode.h
    AdvancedTemplateNode.h
    OperatorNode.h
    ConfigNode.h
    OriginNode.h
)

# 创建静态库
add_library(CHTLNode STATIC ${CHTL_NODE_SOURCES} ${CHTL_NODE_HEADERS})

# 设置包含目录
target_include_directories(CHTLNode PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# 链接依赖
target_link_libraries(CHTLNode PUBLIC
    CHTLLexer
    CHTLError
)

# 设置目标属性
set_target_properties(CHTLNode PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${CHTL_NODE_HEADERS}"
)