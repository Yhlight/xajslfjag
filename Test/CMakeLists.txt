# Test模块 - 测试

# 设置Test源文件
set(TEST_SOURCES
    main.cpp
    # CHTLTest.cpp
    # CHTLJSTest.cpp
    # ScannerTest.cpp
    io_stream_test.cpp
    configuration_manager_test.cpp
    use_statement_test.cpp
    constraint_parser_test.cpp
    import_manager_test.cpp
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

# 创建配置管理器测试可执行文件
add_executable(ConfigurationManagerTest configuration_manager_test.cpp)

# 设置目标属性
set_target_properties(ConfigurationManagerTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接库
target_link_libraries(ConfigurationManagerTest CHTLContext)

# 包含目录
target_include_directories(ConfigurationManagerTest PRIVATE
    ${CMAKE_SOURCE_DIR}/CHTL/CHTLContext
)

# 创建use语句测试可执行文件
add_executable(UseStatementTest use_statement_test.cpp)

# 设置目标属性
set_target_properties(UseStatementTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接库
target_link_libraries(UseStatementTest CHTLContext CHTLParser)

# 包含目录
target_include_directories(UseStatementTest PRIVATE
    ${CMAKE_SOURCE_DIR}/CHTL/CHTLContext
    ${CMAKE_SOURCE_DIR}/CHTL/CHTLParser
)

# 创建约束解析器测试可执行文件
add_executable(ConstraintParserTest constraint_parser_test.cpp)

# 设置目标属性
set_target_properties(ConstraintParserTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接库
target_link_libraries(ConstraintParserTest CHTLContext CHTLParser)

# 包含目录
target_include_directories(ConstraintParserTest PRIVATE
    ${CMAKE_SOURCE_DIR}/CHTL/CHTLContext
    ${CMAKE_SOURCE_DIR}/CHTL/CHTLParser
)

# 创建导入管理器测试可执行文件
add_executable(ImportManagerTest import_manager_test.cpp)

# 设置目标属性
set_target_properties(ImportManagerTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接库
target_link_libraries(ImportManagerTest CHTLContext)

# 包含目录
target_include_directories(ImportManagerTest PRIVATE
    ${CMAKE_SOURCE_DIR}/CHTL/CHTLContext
)

# 创建命名空间管理器测试可执行文件
add_executable(NamespaceManagerTest namespace_manager_test.cpp)

# 设置目标属性
set_target_properties(NamespaceManagerTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接库
target_link_libraries(NamespaceManagerTest CHTLContext)

# 包含目录
target_include_directories(NamespaceManagerTest PRIVATE
    ${CMAKE_SOURCE_DIR}/CHTL/CHTLContext
)

# 创建选择器自动化测试可执行文件
add_executable(SelectorAutomationTest selector_automation_test.cpp)

# 设置目标属性
set_target_properties(SelectorAutomationTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 链接库
target_link_libraries(SelectorAutomationTest CHTLContext)

# 包含目录
target_include_directories(SelectorAutomationTest PRIVATE
    ${CMAKE_SOURCE_DIR}/CHTL/CHTLContext
)