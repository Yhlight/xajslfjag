# CHTL JS编译器构建配置

# 源文件
set(CHTLJS_SOURCES
    CHTLJSContext/CHTLJSContext.cpp
    CHTLJSGenerator/CHTLJSGenerator.cpp
    CHTLJSLexer/CHTLJSLexer.cpp
    CHTLJSLexer/GlobalMap.cpp
    CHTLJSLexer/Token.cpp
    CHTLJSLoader/CHTLJSLoader.cpp
    CHTLJSManage/CHTLJSManage.cpp
    CHTLJSNode/CHTLJSNode.cpp
    CHTLJSParser/CHTLJSParser.cpp
    CHTLJSState/CHTLJSState.cpp
    CJMODSystem/CJMODSystem.cpp
)

# 头文件
set(CHTLJS_HEADERS
    CHTLJSContext/CHTLJSContext.h
    CHTLJSGenerator/CHTLJSGenerator.h
    CHTLJSLexer/CHTLJSLexer.h
    CHTLJSLexer/GlobalMap.h
    CHTLJSLexer/Token.h
    CHTLJSLoader/CHTLJSLoader.h
    CHTLJSManage/CHTLJSManage.h
    CHTLJSNode/CHTLJSNode.h
    CHTLJSParser/CHTLJSParser.h
    CHTLJSState/CHTLJSState.h
    CJMODSystem/CJMODSystem.h
)

# 创建CHTL JS核心库
add_library(chtljs_core STATIC ${CHTLJS_SOURCES} ${CHTLJS_HEADERS})

# 设置目标属性
set_target_properties(chtljs_core PROPERTIES
    OUTPUT_NAME "chtljs_core"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 包含目录
target_include_directories(chtljs_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# 安装规则
install(TARGETS chtljs_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${CHTLJS_HEADERS}
    DESTINATION include/chtljs
)