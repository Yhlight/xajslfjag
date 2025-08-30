# CHTL核心编译器库

# 收集所有源文件
file(GLOB_RECURSE CHTL_SOURCES
    "CHTLLexer/*.cpp"
    "CHTLParser/*.cpp"
    "CHTLGenerator/*.cpp"
    "CHTLContext/*.cpp"
    "CHTLState/*.cpp"
    "CHTLNode/*.cpp"
    "CHTLManage/*.cpp"
    "CMODSystem/*.cpp"
)

file(GLOB_RECURSE CHTL_HEADERS
    "CHTLLexer/*.h"
    "CHTLParser/*.h"
    "CHTLGenerator/*.h"
    "CHTLContext/*.h"
    "CHTLState/*.h"
    "CHTLNode/*.h"
    "CHTLManage/*.h"
    "CMODSystem/*.h"
)

# 创建CHTL核心库
add_library(CHTL_Core STATIC
    ${CHTL_SOURCES}
    ${CHTL_HEADERS}
)

# 设置包含目录
target_include_directories(CHTL_Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# 链接依赖
target_link_libraries(CHTL_Core
    Error_System
    Util_System
)

# 编译选项
target_compile_features(CHTL_Core PUBLIC cxx_std_17)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(CHTL_Core PRIVATE DEBUG_MODE=1)
endif()

message(STATUS "CHTL核心库配置完成")
message(STATUS "  源文件数量: ${CHTL_SOURCES}")
message(STATUS "  头文件数量: ${CHTL_HEADERS}")