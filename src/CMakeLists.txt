# src CMakeLists.txt

# 创建各个库
# Core库
add_library(CHTLCore STATIC
    Core/Common.cpp
    Core/Logger.cpp
    Core/Timer.cpp
    Core/Utils.cpp
)

# Scanner库
add_library(CHTLScanner STATIC
    Scanner/UnifiedScanner.cpp
    Scanner/ScannerUtils.cpp
    Scanner/SliceManager.cpp
)

# CHTL编译器库
add_library(CHTLCompiler STATIC
    CHTL/Token.cpp
    CHTL/Lexer.cpp
    CHTL/Parser.cpp
    CHTL/AST.cpp
    CHTL/Generator.cpp
    CHTL/GlobalMap.cpp
    CHTL/State.cpp
    CHTL/Context.cpp
    CHTL/TemplateSystem.cpp
    CHTL/CustomSystem.cpp
    CHTL/ConfigurationManager.cpp
    CHTL/ImportResolver.cpp
    CHTL/NamespaceManager.cpp
)

# CHTL JS编译器库
add_library(CHTLJSCompiler STATIC
    CHTLJS/Token.cpp
    CHTLJS/Lexer.cpp
    CHTLJS/Parser.cpp
    CHTLJS/AST.cpp
    CHTLJS/Generator.cpp
    CHTLJS/GlobalMap.cpp
    CHTLJS/State.cpp
    CHTLJS/Context.cpp
    CHTLJS/EventHandler.cpp
    CHTLJS/AnimationSystem.cpp
    CHTLJS/VirtualObjectSystem.cpp
)

# CSS编译器库（暂时使用占位文件）
add_library(CSSCompiler STATIC
    CSS/CSSLexer.cpp
    CSS/CSSParser.cpp
    CSS/CSSGenerator.cpp
)

# JavaScript编译器库（暂时使用占位文件）
add_library(JavaScriptCompiler STATIC
    JavaScript/JSLexer.cpp
    JavaScript/JSParser.cpp
    JavaScript/JSGenerator.cpp
)

# 调度器库
add_library(CHTLDispatcher STATIC
    Dispatcher/CompilerDispatcher.cpp
    Dispatcher/ResultMerger.cpp
    Dispatcher/ModuleResolver.cpp
)

# 模块系统库
add_library(CHTLModules STATIC
    Modules/CMODManager.cpp
    Modules/CJMODManager.cpp
    Modules/ModulePacker.cpp
    Modules/ModuleUnpacker.cpp
    Modules/ModuleResolver.cpp
)

# 设置包含目录
target_include_directories(CHTLCore PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(CHTLScanner PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(CHTLCompiler PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(CHTLJSCompiler PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(CSSCompiler PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(JavaScriptCompiler PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(CHTLDispatcher PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(CHTLModules PUBLIC ${CMAKE_SOURCE_DIR}/include)

# 链接依赖
target_link_libraries(CHTLScanner PUBLIC CHTLCore)
target_link_libraries(CHTLCompiler PUBLIC CHTLCore CHTLScanner)
target_link_libraries(CHTLJSCompiler PUBLIC CHTLCore CHTLScanner)
target_link_libraries(CSSCompiler PUBLIC CHTLCore)
target_link_libraries(JavaScriptCompiler PUBLIC CHTLCore)
target_link_libraries(CHTLDispatcher PUBLIC 
    CHTLCore 
    CHTLScanner 
    CHTLCompiler 
    CHTLJSCompiler
    CSSCompiler 
    JavaScriptCompiler
)

# 创建主程序可执行文件
add_executable(chtl main.cpp)

# 链接所有库到主程序
target_link_libraries(chtl PRIVATE
    CHTLCore
    CHTLScanner
    CHTLCompiler
    CHTLJSCompiler
    CSSCompiler
    JavaScriptCompiler
    CHTLDispatcher
    CHTLModules
    Threads::Threads
)