# CHTL JS核心编译器库

# 收集所有源文件
file(GLOB_RECURSE CHTL_JS_SOURCES
    "CHTLJSLexer/*.cpp"
    "CHTLJSParser/*.cpp"
    "CHTLJSGenerator/*.cpp"
    "CHTLJSContext/*.cpp"
    "CHTLJSState/*.cpp"
    "CHTLJSNode/*.cpp"
    "CJMODSystem/*.cpp"
)

file(GLOB_RECURSE CHTL_JS_HEADERS
    "CHTLJSLexer/*.h"
    "CHTLJSParser/*.h"
    "CHTLJSGenerator/*.h"
    "CHTLJSContext/*.h"
    "CHTLJSState/*.h"
    "CHTLJSNode/*.h"
    "CJMODSystem/*.h"
)

# 创建CHTL JS核心库
add_library(CHTL_JS_Core STATIC
    ${CHTL_JS_SOURCES}
    ${CHTL_JS_HEADERS}
)

# 设置包含目录
target_include_directories(CHTL_JS_Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# 链接依赖
target_link_libraries(CHTL_JS_Core
    Error_System
    Util_System
)

# 编译选项
target_compile_features(CHTL_JS_Core PUBLIC cxx_std_17)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(CHTL_JS_Core PRIVATE DEBUG_MODE=1)
endif()

message(STATUS "CHTL JS核心库配置完成")
message(STATUS "  源文件数量: ${CHTL_JS_SOURCES}")
message(STATUS "  头文件数量: ${CHTL_JS_HEADERS}")