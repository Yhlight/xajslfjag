cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# 查找依赖
find_package(Threads REQUIRED)

# ANTLR4运行时
if(WIN32)
    set(ANTLR4_ROOT "${CMAKE_SOURCE_DIR}/../ANTLR4")
    set(ANTLR4_INCLUDE_DIR "${ANTLR4_ROOT}/include")
    set(ANTLR4_LIB_DIR "${ANTLR4_ROOT}/lib")
    set(ANTLR4_LIB "${ANTLR4_LIB_DIR}/antlr4-runtime.lib")
else()
    set(ANTLR4_ROOT "${CMAKE_SOURCE_DIR}/../ANTLR4/linux")
    set(ANTLR4_INCLUDE_DIR "${ANTLR4_ROOT}/include")
    set(ANTLR4_LIB_DIR "${ANTLR4_ROOT}/lib")
    set(ANTLR4_LIB "${ANTLR4_LIB_DIR}/libantlr4-runtime.a")
endif()

if(EXISTS ${ANTLR4_LIB})
    set(USE_ANTLR ON)
    add_definitions(-DUSE_ANTLR)
    message(STATUS "Found ANTLR4 at ${ANTLR4_ROOT}")
else()
    message(WARNING "ANTLR4 not found, using mock implementations")
    set(USE_ANTLR OFF)
endif()

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
if(USE_ANTLR)
    include_directories(${ANTLR4_INCLUDE_DIR})
endif()

# CHTL核心库
add_library(CHTLCore STATIC
    # Scanner
    Scanner/CHTLUnifiedScanner.cpp
    
    # CHTL Compiler
    CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTLLexer/Token.cpp
    CHTL/CHTLLexer/GlobalMap.cpp
    CHTL/CHTLState/State.cpp
    CHTL/CHTLContext/Context.cpp
    CHTL/CHTLNode/BaseNode.cpp
    CHTL/CHTLParser/Parser.cpp
    CHTL/CHTLGenerator/Generator.cpp
    CHTL/CHTLLoader/ImportResolver.cpp
    CHTL/CHTLManage/NamespaceManager.cpp
    CHTL/CHTLManage/SelectorAutomation.cpp
    CHTL/CHTLManage/ConstraintSystem.cpp
    CHTL/CHTLIOStream/CHTLFileSystem.cpp
    CHTL/CMODSystem/CMODPackager.cpp
    
    # CHTL JS Compiler
    CHTLJS/CHTLJSLexer/Lexer.cpp
    CHTLJS/CHTLJSLexer/Token.cpp
    CHTLJS/CHTLJSLexer/GlobalMap.cpp
    CHTLJS/CHTLJSState/State.cpp
    CHTLJS/CHTLJSContext/Context.cpp
    CHTLJS/CHTLJSNode/BaseNode.cpp
    CHTLJS/CHTLJSNode/ModuleNode.cpp
    CHTLJS/CHTLJSParser/Parser.cpp
    CHTLJS/CHTLJSGenerator/Generator.cpp
    CHTLJS/CHTLJSGenerator/ModuleGenerator.cpp
    CHTLJS/CHTLJSLoader/CJMODLoader.cpp
    CHTLJS/CHTLJSManage/VirtualObjectManager.cpp
    CHTLJS/CHTLJSIOStream/CHTLJSFileSystem.cpp
    CHTLJS/CJMODSystem/CJMODPackager.cpp
    CHTLJS/CJMODSystem/CJMODScanner.cpp
    CHTLJS/CJMODSystem/Runtime/CJMODRuntime.cpp
    CHTLJS/CJMODSystem/API/CJMODApi.cpp
    
    # CSS & JS Compilers
    CSS/CSSCompiler.cpp
    JS/JavaScriptCompiler.cpp
    
    # Compiler Dispatcher
    CompilerDispatcher/CompilerDispatcher.cpp
    
    # Utilities
    Util/ZIPUtil/ZIPUtil.cpp
    
    # Error handling
    Error/ErrorReport.cpp
)

# 如果使用ANTLR，添加生成的文件
if(USE_ANTLR)
    target_sources(CHTLCore PRIVATE
        # CSS ANTLR生成的文件
        CSS/generated/src/CSS/CSSSimpleLexer.cpp
        CSS/generated/src/CSS/CSSSimpleParser.cpp
        CSS/generated/src/CSS/CSSSimpleBaseVisitor.cpp
        CSS/generated/src/CSS/CSSSimpleVisitor.cpp
        
        # JavaScript ANTLR生成的文件
        JS/generated/Grammars/JavaScript/JavaScriptLexer.cpp
        JS/generated/Grammars/JavaScript/JavaScriptParser.cpp
        JS/generated/Grammars/JavaScript/JavaScriptParserBaseVisitor.cpp
        JS/generated/Grammars/JavaScript/JavaScriptParserVisitor.cpp
    )
endif()

target_link_libraries(CHTLCore PUBLIC Threads::Threads)

if(USE_ANTLR)
    target_link_libraries(CHTLCore PUBLIC ${ANTLR4_LIB})
endif()

# CHTL编译器可执行文件
add_executable(chtlc
    CHTL/main.cpp
)

target_link_libraries(chtlc PRIVATE CHTLCore)

# CHTL测试套件
if(BUILD_TESTS)
    add_executable(chtl_tests
        Test/main.cpp
        Test/CHTLTestSuite.cpp
        Test/CHTLSyntaxTests.cpp
        Test/CHTLJSSyntaxTests.cpp
        Test/UtilTest/StringUtilTest.cpp
        Test/UtilTest/FileSystemTest.cpp
        Test/UtilTest/ErrorReportTest.cpp
        Test/TokenTestUtil/TokenPrint.cpp
        Test/ASTTestUtil/ASTPrint.cpp
        Test/CompilationMonitor/CompilationMonitor.cpp
    )
    
    target_link_libraries(chtl_tests PRIVATE CHTLCore)
    
    # 添加测试
    enable_testing()
    add_test(NAME CHTLTests COMMAND chtl_tests)
endif()

# CMOD打包工具
add_executable(cmod_pack
    Tools/cmod_pack.cpp
)

target_link_libraries(cmod_pack PRIVATE CHTLCore)

# CJMOD打包工具
add_executable(cjmod_pack
    Tools/cjmod_pack.cpp
)

target_link_libraries(cjmod_pack PRIVATE CHTLCore)

# 安装规则
install(TARGETS chtlc cmod_pack cjmod_pack
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Module/
    DESTINATION share/chtl/modules
    FILES_MATCHING PATTERN "*.chtl" PATTERN "*.cmod" PATTERN "*.cjmod"
)

# 创建模块目录结构
install(DIRECTORY DESTINATION share/chtl/modules/CMOD)
install(DIRECTORY DESTINATION share/chtl/modules/CJMOD)

# CPack配置
set(CPACK_PACKAGE_NAME "CHTL")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ HyperText Language Compiler")
set(CPACK_PACKAGE_VENDOR "CHTL Project")

include(CPack)