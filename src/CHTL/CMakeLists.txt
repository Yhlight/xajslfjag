# CMakeLists.txt for CHTL Project
cmake_minimum_required(VERSION 3.16)

# 设置项目信息
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 添加子目录
add_subdirectory(../Error CHTLError)
add_subdirectory(../Util/FileSystem CHTLFileSystem)
add_subdirectory(CHTLLexer)
add_subdirectory(CHTLState)
add_subdirectory(CHTLContext)
add_subdirectory(CHTLNode)
add_subdirectory(CHTLIOStream)
add_subdirectory(CHTLParser)
add_subdirectory(CHTLGenerator)
add_subdirectory(CHTLManage)
add_subdirectory(CHTLScanner)
add_subdirectory(CHTLCompiler)
add_subdirectory(CJMOD)
add_subdirectory(../Test CHTLTest)

# 如果需要构建测试程序
option(BUILD_TESTS "Build test programs" ON)

if(BUILD_TESTS)
    # 创建综合测试程序
    add_executable(test_state_context test_state_context.cpp)
    target_link_libraries(test_state_context 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLTest
    )
    
    # 创建更新后的lexer测试程序
    add_executable(test_updated_lexer test_updated_lexer.cpp)
    target_link_libraries(test_updated_lexer 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
    )
    
    # 创建完整系统测试程序
    add_executable(test_complete_system test_complete_system.cpp)
    target_link_libraries(test_complete_system 
        CHTLFileSystem
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLIOStream
        CHTLParser
        CHTLGenerator
        CHTLManage
        CHTLTest
    )
    
    # 创建CHTL语法测试程序
    add_executable(test_chtl_syntax test_chtl_syntax.cpp)
    target_link_libraries(test_chtl_syntax 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    # 创建CHTL JS系统测试程序
    add_executable(test_chtl_js_system test_chtl_js_system.cpp)
    target_link_libraries(test_chtl_js_system 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    # 创建选择器自动化测试程序
    add_executable(test_selector_automation test_selector_automation.cpp)
    target_link_libraries(test_selector_automation 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    # 创建模板特化操作测试程序
    add_executable(test_template_specialization test_template_specialization.cpp)
    target_link_libraries(test_template_specialization 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    # 创建变量系统测试程序
    add_executable(test_variable_system test_variable_system.cpp)
    target_link_libraries(test_variable_system 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    # 创建统一系统测试程序
    add_executable(test_unified_system test_unified_system.cpp)
    target_link_libraries(test_unified_system 
        CHTLError
        CHTLScanner
        CHTLCompiler
        CHTLManage
        CHTLParser
        CHTLGenerator
        CHTLNode
        CHTLContext
        CHTLState
    )
    
    # 创建完整CHTL特性测试程序
    add_executable(test_complete_chtl_features test_complete_chtl_features.cpp)
    target_link_libraries(test_complete_chtl_features 
        CHTLError
        CHTLScanner
        CHTLCompiler
        CHTLManage
        CHTLParser
        CHTLGenerator
        CHTLNode
        CHTLContext
        CHTLState
    )
    
    # 设置包含目录
    target_include_directories(test_state_context PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/../Test
    )
    
    target_include_directories(test_updated_lexer PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/../Test
    )
    
    target_include_directories(test_complete_system PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Util/FileSystem
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLIOStream
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLGenerator
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLManage
        ${CMAKE_CURRENT_SOURCE_DIR}/../Test
    )
    
    target_include_directories(test_chtl_syntax PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLManage
    )
    
    # 创建模板系统测试程序
    add_executable(test_template_system test_template_system.cpp)
    target_link_libraries(test_template_system 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    target_include_directories(test_template_system PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLManage
    )
    
    # 创建完整模板系统测试程序
    add_executable(test_complete_template_system test_complete_template_system.cpp)
    target_link_libraries(test_complete_template_system 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    target_include_directories(test_complete_template_system PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLManage
    )
    
    # 创建原始嵌入系统测试程序
    add_executable(test_origin_system test_origin_system.cpp)
    target_link_libraries(test_origin_system 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    target_include_directories(test_origin_system PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLManage
    )
    
    # 创建配置系统测试程序
    add_executable(test_configuration_system test_configuration_system.cpp)
    target_link_libraries(test_configuration_system 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    target_include_directories(test_configuration_system PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLManage
    )
    
    # 创建配置Token调试程序
    add_executable(debug_config_tokens debug_config_tokens.cpp)
    target_link_libraries(debug_config_tokens 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
    )
    
    target_include_directories(debug_config_tokens PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
    )
    
    # 创建配置调试程序
    add_executable(test_config_debug test_config_debug.cpp)
    target_link_libraries(test_config_debug 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
    )
    
    target_include_directories(test_config_debug PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
    )
    
    # 创建简单解析调试程序
    add_executable(debug_parse_simple debug_parse_simple.cpp)
    target_link_libraries(debug_parse_simple 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
    )
    
    target_include_directories(debug_parse_simple PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
    )
    
    # 创建导入系统测试程序
    add_executable(test_import_system test_import_system.cpp)
    target_link_libraries(test_import_system 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
    )
    
    target_include_directories(test_import_system PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
    )
    
    # 创建命名空间系统测试程序
    add_executable(test_namespace_system test_namespace_system.cpp)
    target_link_libraries(test_namespace_system 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    target_include_directories(test_namespace_system PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLManage
    )

    # 简单命名空间测试
    add_executable(test_simple_namespace test_simple_namespace.cpp)
    target_link_libraries(test_simple_namespace 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    target_include_directories(test_simple_namespace PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLManage
    )

    # 命名空间词法调试
    add_executable(debug_namespace_tokens debug_namespace_tokens.cpp)
    target_link_libraries(debug_namespace_tokens 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    target_include_directories(debug_namespace_tokens PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLManage
    )

    # 命名空间解析调试
    add_executable(debug_namespace_parser debug_namespace_parser.cpp)
    target_link_libraries(debug_namespace_parser 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    target_include_directories(debug_namespace_parser PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLManage
    )

    # 详细命名空间解析调试
    add_executable(debug_namespace_parsing_detailed debug_namespace_parsing_detailed.cpp)
    target_link_libraries(debug_namespace_parsing_detailed 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    target_include_directories(debug_namespace_parsing_detailed PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLManage
    )

    # 简单自定义节点测试
    add_executable(test_simple_custom test_simple_custom.cpp)
    target_link_libraries(test_simple_custom 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    target_include_directories(test_simple_custom PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLManage
    )

    # 自定义元素解析调试
    add_executable(debug_custom_element_parsing debug_custom_element_parsing.cpp)
    target_link_libraries(debug_custom_element_parsing 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    target_include_directories(debug_custom_element_parsing PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLManage
    )

    # 约束系统测试
    add_executable(test_constraint_system test_constraint_system.cpp)
    target_link_libraries(test_constraint_system 
        CHTLError
        CHTLLexer 
        CHTLState 
        CHTLContext
        CHTLNode
        CHTLParser
        CHTLManage
    )
    
    target_include_directories(test_constraint_system PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Error
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLState
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLContext
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser
        ${CMAKE_CURRENT_SOURCE_DIR}/CHTLManage
    )
    
    # 最终简化测试程序
    add_executable(test_final_simple test_final_simple.cpp)
    target_link_libraries(test_final_simple 
        CHTLError
        CHTLScanner
        CHTLCompiler
        CHTLManage
        CHTLParser
        CHTLGenerator
        CHTLNode
        CHTLContext
        CHTLState
        CHTLCJMOD
    )
    
    # 短期目标完整验证测试
    add_executable(test_short_term_goals_simple test_short_term_goals_simple.cpp)
    target_link_libraries(test_short_term_goals_simple 
        CHTLError
        CHTLScanner
        CHTLCompiler
        CHTLManage
        CHTLParser
        CHTLGenerator
        CHTLNode
        CHTLContext
        CHTLState
        CHTLLexer
        CHTLCJMOD
    )
    
    # CJMOD运行时系统测试
    add_executable(test_cjmod_runtime test_cjmod_runtime.cpp)
    target_link_libraries(test_cjmod_runtime 
        CHTLError
        CHTLCJMOD
    )
    
    # CJMOD完整集成测试
    add_executable(test_cjmod_complete_integration test_cjmod_complete_integration.cpp)
    target_link_libraries(test_cjmod_complete_integration 
        CHTLError
        CHTLCJMOD
    )
    
endif()