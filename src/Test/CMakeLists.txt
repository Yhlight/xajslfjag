# 测试系统库

file(GLOB_RECURSE TEST_SOURCES
    "TokenTest/*.cpp"
    "ASTTest/*.cpp"
    "CompilerTimeMonitor/*.cpp"
)

file(GLOB_RECURSE TEST_HEADERS
    "TokenTest/*.h"
    "ASTTest/*.h"
    "CompilerTimeMonitor/*.h"
)

if(TEST_SOURCES)
    add_library(Test_System STATIC
        ${TEST_SOURCES}
        ${TEST_HEADERS}
    )

    target_include_directories(Test_System PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_compile_features(Test_System PUBLIC cxx_std_17)

    message(STATUS "测试系统库配置完成")
else()
    message(STATUS "测试系统库跳过（无源文件）")
endif()