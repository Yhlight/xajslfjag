cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译器特定配置
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

# 调试配置
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DCHTL_DEBUG)
endif()

# 包含目录
include_directories(${PROJECT_SOURCE_DIR})

# CHTL核心库
set(CHTL_CORE_SOURCES
    # CHTLLexer模块
    CHTL/CHTLLexer/Token.cpp
    CHTL/CHTLLexer/GlobalMap.cpp
    CHTL/CHTLLexer/Lexer.cpp
    
    # CHTLState模块
    CHTL/CHTLState/CHTLState.cpp
    
    # CHTLNode模块
    CHTL/CHTLNode/BaseNode.cpp
    CHTL/CHTLNode/CommentNode.cpp
    # CHTL/CHTLNode/TemplateNode.cpp
    # CHTL/CHTLNode/CustomNode.cpp
    # CHTL/CHTLNode/StyleNode.cpp
    # CHTL/CHTLNode/ScriptNode.cpp
    # CHTL/CHTLNode/OriginNode.cpp
    # CHTL/CHTLNode/ImportNode.cpp
    # CHTL/CHTLNode/ConfigNode.cpp
    # CHTL/CHTLNode/NamespaceNode.cpp
    # CHTL/CHTLNode/OperatorNode.cpp
    
    # CHTLContext模块
    # CHTL/CHTLContext/CHTLContext.cpp
    
    # CHTLParser模块
    # CHTL/CHTLParser/CHTLParser.cpp
    
    # CHTLGenerator模块
    # CHTL/CHTLGenerator/CHTLGenerator.cpp
    
    # CHTLLoader模块
    # CHTL/CHTLLoader/CHTLLoader.cpp
    
    # CHTLManage模块
    # CHTL/CHTLManage/CHTLManage.cpp
    
    # CMODSystem模块
    # CHTL/CMODSystem/CMODSystem.cpp
)

# 创建CHTL核心库
add_library(chtl_core STATIC ${CHTL_CORE_SOURCES})

# 设置库的包含目录
target_include_directories(chtl_core PUBLIC ${PROJECT_SOURCE_DIR})

# 编译器基础设施库
set(CHTL_INFRASTRUCTURE_SOURCES
    # Scanner模块
    # Scanner/Scanner.cpp
    
    # CompilerDispatcher模块
    # CompilerDispatcher/CompilerDispatcher.cpp
    
    # CSS编译器
    # CSS/CSSCompiler.cpp
    
    # JS编译器  
    # JS/JSCompiler.cpp
)

# 创建基础设施库（暂时注释掉，因为源文件还未实现）
# add_library(chtl_infrastructure STATIC ${CHTL_INFRASTRUCTURE_SOURCES})
# target_include_directories(chtl_infrastructure PUBLIC ${PROJECT_SOURCE_DIR})
# target_link_libraries(chtl_infrastructure chtl_core)

# 工具库
set(CHTL_UTIL_SOURCES
    # Util/FileSystem/FileSystem.cpp
)

# 创建工具库（暂时注释掉）
# add_library(chtl_util STATIC ${CHTL_UTIL_SOURCES})
# target_include_directories(chtl_util PUBLIC ${PROJECT_SOURCE_DIR})

# 测试程序
add_executable(chtl_test Test/SimpleTestMain.cpp)
target_link_libraries(chtl_test chtl_core)

# CHTL JS库（未来实现）
set(CHTL_JS_SOURCES
    # "CHTL JS"模块的源文件将在这里添加
)

# 创建CHTL JS库（暂时注释掉）
# add_library(chtl_js STATIC ${CHTL_JS_SOURCES})
# target_include_directories(chtl_js PUBLIC ${PROJECT_SOURCE_DIR})

# 主编译器可执行文件（未来实现）
# add_executable(chtl_compiler Main/main.cpp)
# target_link_libraries(chtl_compiler chtl_core chtl_infrastructure chtl_util chtl_js)

# 安装配置
install(TARGETS chtl_core DESTINATION lib)
install(TARGETS chtl_test DESTINATION bin)

# 安装头文件
install(DIRECTORY CHTL/ DESTINATION include/CHTL 
        FILES_MATCHING PATTERN "*.hpp")

# CPack配置
set(CPACK_PACKAGE_NAME "CHTL")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CHTL - C++ HTML Template Language")
set(CPACK_PACKAGE_VENDOR "CHTL Team")

include(CPack)

# 添加自定义目标用于代码格式化（如果有clang-format）
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT} -i -style=file ${CHTL_CORE_SOURCES}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Formatting code with clang-format"
    )
endif()

# 添加文档生成目标（如果有Doxygen）
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${PROJECT_SOURCE_DIR}/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(docs
        ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMENT "Generating documentation with Doxygen"
    )
endif()

# 测试配置
enable_testing()
add_test(NAME chtl_basic_test COMMAND chtl_test)

# 设置测试环境
set_tests_properties(chtl_basic_test PROPERTIES
    ENVIRONMENT "CHTL_TEST_MODE=1"
)

# 性能测试（未来添加）
# add_test(NAME chtl_performance_test COMMAND chtl_test --performance)

# 内存检查（如果有valgrind）
find_program(VALGRIND valgrind)
if(VALGRIND)
    add_custom_target(memcheck
        COMMAND ${VALGRIND} --tool=memcheck --leak-check=full --show-leak-kinds=all 
                --track-origins=yes --verbose ./chtl_test
        DEPENDS chtl_test
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMENT "Running memory check with valgrind"
    )
endif()

# 代码覆盖率（如果有gcov/lcov）
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
        
        find_program(LCOV lcov)
        find_program(GENHTML genhtml)
        
        if(LCOV AND GENHTML)
            add_custom_target(coverage
                COMMAND ${LCOV} --directory . --capture --output-file coverage.info
                COMMAND ${LCOV} --remove coverage.info '/usr/*' --output-file coverage.info
                COMMAND ${LCOV} --list coverage.info
                COMMAND ${GENHTML} coverage.info --output-directory coverage_html
                WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                COMMENT "Generating code coverage report"
            )
        endif()
    endif()
endif()

message(STATUS "CHTL项目配置完成!")
message(STATUS "  - 版本: ${PROJECT_VERSION}")
message(STATUS "  - 构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "  - C++标准: ${CMAKE_CXX_STANDARD}")
message(STATUS "  - 编译器: ${CMAKE_CXX_COMPILER_ID}")