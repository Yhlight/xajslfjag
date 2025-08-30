cmake_minimum_required(VERSION 3.16)
project(CHTL_Compiler VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Wextra")
    add_definitions(-DDEBUG_MODE=1)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
endif()

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 添加子目录 - 按依赖顺序
add_subdirectory(Error)
add_subdirectory(Util)
add_subdirectory(ThirdParty)

# 核心系统
add_subdirectory(CHTL)
add_subdirectory("CHTL JS")
add_subdirectory(Scanner)
add_subdirectory(CompilerDispatcher)

# CSS和JS编译器
add_subdirectory(CSS)
add_subdirectory(JS)

# 测试系统
add_subdirectory(Test)

# 主程序
add_executable(chtl_compiler
    main.cpp
)

# 链接所有库
target_link_libraries(chtl_compiler
    CHTL_Core
    CHTL_JS_Core
    Scanner_Core
    CompilerDispatcher_Core
    CSS_Compiler
    JS_Compiler
    Error_System
    Util_System
)

# 测试程序
add_executable(test_suite
    test_suite.cpp
)

target_link_libraries(test_suite
    CHTL_Core
    CHTL_JS_Core
    Scanner_Core
    CompilerDispatcher_Core
    CSS_Compiler
    JS_Compiler
    Error_System
    Util_System
)

# 完整实现验证程序
add_executable(test_complete_implementation
    test_complete_chtl_implementation.cpp
)

target_link_libraries(test_complete_implementation
    CHTL_Core
    CHTL_JS_Core
    Scanner_Core
    CompilerDispatcher_Core
    Error_System
)

# ANTLR4集成测试
add_executable(test_antlr_integration
    test_antlr_libraries.cpp
)

target_link_libraries(test_antlr_integration
    CSS_Compiler
    JS_Compiler
    ${ANTLR4_LIBRARIES}
)

# 安装规则
install(TARGETS chtl_compiler
    RUNTIME DESTINATION bin
)

install(DIRECTORY Module/
    DESTINATION share/chtl/modules
    FILES_MATCHING PATTERN "*.chtl" PATTERN "*.cmod" PATTERN "*.cjmod"
)

# CPack配置
set(CPACK_PACKAGE_NAME "CHTL-Compiler")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CHTL超文本编译器")
set(CPACK_PACKAGE_VENDOR "CHTL Team")

include(CPack)

# 打印构建信息
message(STATUS "CHTL编译器构建配置:")
message(STATUS "  版本: ${PROJECT_VERSION}")
message(STATUS "  C++标准: ${CMAKE_CXX_STANDARD}")
message(STATUS "  构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "  编译器: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  平台: ${CMAKE_SYSTEM_NAME}")

# 功能特性总结
message(STATUS "")
message(STATUS "CHTL编译器功能特性:")
message(STATUS "  ✓ 完整的CHTL语法支持")
message(STATUS "  ✓ 强大的CHTL JS增强语法")
message(STATUS "  ✓ 模块化开发支持(CMOD/CJMOD)")
message(STATUS "  ✓ 官方模块(Chtholly/Yuigahama)")
message(STATUS "  ✓ ANTLR4集成(CSS/JS编译器)")
message(STATUS "  ✓ 跨平台支持(Windows/Linux)")
message(STATUS "  ✓ 高性能统一扫描器")
message(STATUS "  ✓ 智能编译器调度")
message(STATUS "")