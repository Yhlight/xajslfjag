# Test模块 - 测试

# 设置Test源文件
set(TEST_SOURCES
    main.cpp
    # CHTLTest.cpp
    # CHTLJSTest.cpp
    # ScannerTest.cpp
    io_stream_test.cpp
)

# 创建Test可执行文件
add_executable(CHTLTest ${TEST_SOURCES})

# 设置目标属性
set_target_properties(CHTLTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接库
target_link_libraries(CHTLTest 
    Scanner 
    CHTLContext 
    CompilerDispatcher
    CHTLParser
    CHTLJSLexer
    CHTLJSParser
    CHTLIOStream
    CHTLJSIOStream
    FileStream
    ZipUtil
)

# 包含目录
target_include_directories(CHTLTest PRIVATE
    ${CMAKE_SOURCE_DIR}/Scanner
    ${CMAKE_SOURCE_DIR}/ThirdParty/CJMODAPI
    ${CMAKE_SOURCE_DIR}/CHTL/CHTLContext
)