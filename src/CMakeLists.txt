# CHTL核心库
add_library(CHTLCore STATIC
    Core/FileSystem.cpp
    Core/Logger.cpp
    Core/Timer.cpp
    Utils/StringUtils.cpp
    Utils/PathUtils.cpp
)

# 统一扫描器
add_library(CHTLScanner STATIC
    Scanner/UnifiedScanner.cpp
    Scanner/SliceManager.cpp
    Scanner/TokenIdentifier.cpp
)

# CHTL编译器
add_library(CHTLCompiler STATIC
    CHTL/Token.cpp
    CHTL/Lexer.cpp
    CHTL/Parser.cpp
    CHTL/AST.cpp
    CHTL/Generator.cpp
    CHTL/Context.cpp
    CHTL/State.cpp
    CHTL/GlobalMap.cpp
    CHTL/TemplateSystem.cpp
    CHTL/CustomSystem.cpp
    CHTL/ImportResolver.cpp
    CHTL/NamespaceManager.cpp
    CHTL/ConfigurationManager.cpp
)

# CHTL JS编译器
add_library(CHTLJSCompiler STATIC
    CHTLJS/Token.cpp
    CHTLJS/Lexer.cpp
    CHTLJS/Parser.cpp
    CHTLJS/AST.cpp
    CHTLJS/Generator.cpp
    CHTLJS/Context.cpp
    CHTLJS/State.cpp
    CHTLJS/GlobalMap.cpp
    CHTLJS/SelectorEngine.cpp
    CHTLJS/ListenerEngine.cpp
    CHTLJS/AnimationEngine.cpp
    CHTLJS/VirtualObjectManager.cpp
)

# CSS编译器（ANTLR）
if(EXISTS ${ANTLR4_ROOT})
    add_library(CSSCompiler STATIC
        CSS/CSSCompiler.cpp
        CSS/CSSLexer.cpp
        CSS/CSSParser.cpp
        CSS/CSSGenerator.cpp
    )
    target_link_libraries(CSSCompiler antlr4_static)
endif()

# JavaScript编译器（ANTLR）
if(EXISTS ${ANTLR4_ROOT})
    add_library(JavaScriptCompiler STATIC
        JavaScript/JavaScriptCompiler.cpp
        JavaScript/JavaScriptLexer.cpp
        JavaScript/JavaScriptParser.cpp
        JavaScript/JavaScriptGenerator.cpp
    )
    target_link_libraries(JavaScriptCompiler antlr4_static)
endif()

# 编译器调度器
add_library(CHTLDispatcher STATIC
    Dispatcher/CompilerDispatcher.cpp
    Dispatcher/ResultMerger.cpp
)

# 模块系统
add_library(CHTLModules STATIC
    Modules/CMODManager.cpp
    Modules/CJMODManager.cpp
    Modules/ModulePacker.cpp
    Modules/ModuleUnpacker.cpp
    Modules/ModuleResolver.cpp
)

# CHTL主程序
add_executable(chtl
    main.cpp
)

# 链接库
target_link_libraries(chtl
    CHTLCore
    CHTLScanner
    CHTLCompiler
    CHTLJSCompiler
    CHTLDispatcher
    CHTLModules
    Threads::Threads
)

# 如果ANTLR存在，链接CSS和JS编译器
if(EXISTS ${ANTLR4_ROOT})
    target_link_libraries(chtl CSSCompiler JavaScriptCompiler)
endif()

# 设置可执行文件输出名称
set_target_properties(chtl PROPERTIES OUTPUT_NAME "chtl")

# Windows特定设置
if(WIN32)
    set_target_properties(chtl PROPERTIES SUFFIX ".exe")
endif()