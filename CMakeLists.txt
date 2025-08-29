cmake_minimum_required(VERSION 3.15)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译器特定的标志
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /MD /DNDEBUG")
endif()

# 查找必要的包
find_package(Threads REQUIRED)

# 定义头文件目录
set(CHTL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Util
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTL
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTL/CHTLLexer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTL/CHTLParser
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTL/CHTLNode
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTL/CHTLGenerator
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTL/CHTLLoader
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTL/CHTLManage
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTL/CHTLState
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTL/CHTLConfig
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTLJS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTLJS/CHTLJSLexer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTLJS/CHTLJSParser
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTLJS/CHTLJSNode
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTLJS/CHTLJSGenerator
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Scanner
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CompilerDispatcher
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CMODSystem
)

# 定义所有源文件
set(CHTL_CORE_SOURCES
    # Util
    src/Util/Common.cpp
    
    # CHTL Core
    src/CHTL/CHTLLexer/Token.cpp
    src/CHTL/CHTLLexer/GlobalMap.cpp
    src/CHTL/CHTLLexer/Lexer.cpp
    src/CHTL/CHTLParser/Parser.cpp
    src/CHTL/CHTLNode/BaseNode.cpp
    src/CHTL/CHTLNode/TemplateNode.cpp
    src/CHTL/CHTLNode/CustomNode.cpp
    src/CHTL/CHTLNode/OriginNode.cpp
    src/CHTL/CHTLGenerator/Generator.cpp
    src/CHTL/CHTLLoader/ModuleLoader.cpp
    src/CHTL/CHTLManage/NamespaceResolver.cpp
    src/CHTL/CHTLManage/ConstraintValidator.cpp
    src/CHTL/CHTLState/RAIIManager.cpp
    
    # CHTL JS
    src/CHTLJS/CHTLJSLexer/Lexer.cpp
    src/CHTLJS/CHTLJSParser/Parser.cpp
    src/CHTLJS/CHTLJSGenerator/Generator.cpp
    
    # Scanner and Dispatcher
    src/Scanner/CHTLUnifiedScanner.cpp
    src/CompilerDispatcher/Dispatcher.cpp
    
    # CMOD System
    src/CMODSystem/CJMODScanner.cpp
)

# 定义所有头文件
set(CHTL_CORE_HEADERS
    # Util
    src/Util/Common.h
    
    # CHTL Core
    src/CHTL/CHTLLexer/Token.h
    src/CHTL/CHTLLexer/GlobalMap.h
    src/CHTL/CHTLLexer/Lexer.h
    src/CHTL/CHTLParser/Parser.h
    src/CHTL/CHTLNode/BaseNode.h
    src/CHTL/CHTLNode/TemplateNode.h
    src/CHTL/CHTLNode/CustomNode.h
    src/CHTL/CHTLNode/OriginNode.h
    src/CHTL/CHTLGenerator/Generator.h
    src/CHTL/CHTLLoader/ModuleLoader.h
    src/CHTL/CHTLManage/NamespaceResolver.h
    src/CHTL/CHTLManage/ConstraintValidator.h
    src/CHTL/CHTLState/RAIIManager.h
    src/CHTL/CHTLConfig/ConfigNode.h
    
    # CHTL JS
    src/CHTLJS/CHTLJSLexer/Lexer.h
    src/CHTLJS/CHTLJSParser/Parser.h
    src/CHTLJS/CHTLJSNode/CHTLJSNodes.h
    src/CHTLJS/CHTLJSGenerator/Generator.h
    
    # Scanner and Dispatcher
    src/Scanner/CHTLUnifiedScanner.h
    src/CompilerDispatcher/Dispatcher.h
    
    # CMOD System
    src/CMODSystem/CJMODScanner.h
)

# 创建CHTL核心库
add_library(chtl_core STATIC ${CHTL_CORE_SOURCES} ${CHTL_CORE_HEADERS})

# 设置包含目录
target_include_directories(chtl_core PUBLIC ${CHTL_INCLUDE_DIRS})

# 链接线程库
target_link_libraries(chtl_core PUBLIC Threads::Threads)

# 设置编译器特定的选项
target_compile_features(chtl_core PUBLIC cxx_std_17)

# 定义预处理器宏
target_compile_definitions(chtl_core PRIVATE
    CHTL_VERSION_MAJOR=1
    CHTL_VERSION_MINOR=0
    CHTL_VERSION_PATCH=0
)

# 如果是Debug模式，启用调试宏
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(chtl_core PRIVATE CHTL_DEBUG)
endif()

# 创建CHTL编译器可执行文件
add_executable(chtl_compiler
    src/main.cpp
)

target_link_libraries(chtl_compiler PRIVATE chtl_core)
target_include_directories(chtl_compiler PRIVATE ${CHTL_INCLUDE_DIRS})

# 创建测试可执行文件
set(TEST_SOURCES
    test_basic_syntax.cpp
    test_token_recognition.cpp
    test_element_parsing.cpp
    test_simple_parsing.cpp
    test_template_system.cpp
    test_custom_system.cpp
    test_origin_system.cpp
    test_config_system.cpp
    test_chtl_complete_syntax.cpp
    test_complete_integration.cpp
)

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE chtl_core)
    target_include_directories(${TEST_NAME} PRIVATE ${CHTL_INCLUDE_DIRS})
endforeach()

# 设置安装目标
install(TARGETS chtl_core chtl_compiler
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# 安装头文件
install(DIRECTORY src/
    DESTINATION include/chtl
    FILES_MATCHING PATTERN "*.h"
)

# 创建包配置文件
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CHTLConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CHTLConfig.cmake"
    INSTALL_DESTINATION lib/cmake/CHTL
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CHTLConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CHTLConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CHTLConfigVersion.cmake"
    DESTINATION lib/cmake/CHTL
)

# 导出目标
export(TARGETS chtl_core FILE "${CMAKE_CURRENT_BINARY_DIR}/CHTLTargets.cmake")
install(EXPORT CHTLTargets
    FILE CHTLTargets.cmake
    DESTINATION lib/cmake/CHTL
)

# 创建自定义目标用于运行测试
add_custom_target(run_tests
    COMMENT "Running CHTL tests"
)

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_custom_command(
        TARGET run_tests POST_BUILD
        COMMAND ${TEST_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running ${TEST_NAME}"
        DEPENDS ${TEST_NAME}
    )
endforeach()

# 打包配置
set(CPACK_PACKAGE_NAME "CHTL")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CHTL - Hypertext Language Compiler")
set(CPACK_PACKAGE_VENDOR "CHTL Project")
set(CPACK_PACKAGE_CONTACT "chtl@example.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ;DragNDrop")
else()
    set(CPACK_GENERATOR "TGZ;DEB;RPM")
endif()

include(CPack)

# 显示配置信息
message(STATUS "CHTL Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Source Files: ${CHTL_CORE_SOURCES}")
message(STATUS "  Header Files: ${CHTL_CORE_HEADERS}")
message(STATUS "  Test Files: ${TEST_SOURCES}")

# 性能和调试选项
option(CHTL_ENABLE_PROFILING "Enable profiling support" OFF)
option(CHTL_ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)
option(CHTL_BUILD_SHARED_LIBS "Build CHTL as shared libraries" OFF)

if(CHTL_ENABLE_PROFILING)
    target_compile_options(chtl_core PRIVATE -pg)
    target_link_options(chtl_core PRIVATE -pg)
    message(STATUS "Profiling enabled")
endif()

if(CHTL_ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(chtl_core PRIVATE -fsanitize=address,undefined)
    target_link_options(chtl_core PRIVATE -fsanitize=address,undefined)
    message(STATUS "Sanitizers enabled")
endif()

if(CHTL_BUILD_SHARED_LIBS)
    set_target_properties(chtl_core PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
    message(STATUS "Building shared libraries")
endif()