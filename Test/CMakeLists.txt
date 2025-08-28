# Test模块 - 测试

# 设置Test源文件
set(TEST_SOURCES
    main.cpp
    # CHTLTest.cpp
    # CHTLJSTest.cpp
    # ScannerTest.cpp
)

# 创建Test可执行文件
add_executable(CHTLTest ${TEST_SOURCES})

# 设置目标属性
set_target_properties(CHTLTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接依赖库 - 暂时只链接Scanner
target_link_libraries(CHTLTest
    # CompilerDispatcher
    Scanner
    # CHTL
    # CHTLJS
    # CSS
    # JS
    # Util
)