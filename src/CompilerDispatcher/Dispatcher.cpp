#include "Dispatcher.h"
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include "../CHTLJS/CHTLJSParser/Parser.h"
#include "../CHTLJS/CHTLJSGenerator/Generator.h"
#include "../ANTLR/ANTLRJavaScriptWrapper.h"
#include "../ANTLR/ANTLRCSSWrapper.h"
#include <algorithm>
#include <future>
#include <thread>

namespace CHTL {

// CompilerDispatcher实现
CompilerDispatcher::CompilerDispatcher(const DispatcherConfig& config)
    : config(config), cacheEnabled(config.enableCaching), debugMode(false), shouldStop(false) {
    // 初始化ANTLR组件
    antlrJSWrapper = std::make_unique<ANTLR::ANTLRJavaScriptWrapper>();
    antlrCSSWrapper = std::make_unique<ANTLR::ANTLRCSSWrapper>();
    
    // 初始化其他组件
    scanner = std::make_unique<CHTLUnifiedScanner>();
    chtlParser = std::make_unique<Parser>();
    generator = std::make_unique<Generator>();
    
    // 启动工作线程
    if (config.maxConcurrentTasks > 0) {
        startWorkerThreads();
    }
}

CompilerDispatcher::~CompilerDispatcher() {
    cleanup();
}

CompilationResult CompilerDispatcher::compile(const String& sourceCode) {
    CompilationResult result;
    result.success = false;
    
    try {
        // 1. 使用统一扫描器进行代码切割
        auto scanner = std::make_unique<CHTLUnifiedScanner>(sourceCode);
        auto fragments = scanner->scanAndSplit();
        
        if (scanner->hasErrors()) {
            for (const auto& error : scanner->getErrors()) {
                result.errors.push_back("Scanner Error: " + error);
            }
            return result;
        }
        
        totalFragments = fragments.size();
        processedFragments = 0;
        
        // 2. 调度fragments到相应编译器
        std::vector<std::future<FragmentResult>> futures;
        
        for (const auto& fragment : fragments) {
            if (config.enableParallelCompilation) {
                // 并行编译
                futures.push_back(std::async(std::launch::async, 
                    [this, fragment]() { return compileFragment(fragment); }));
            } else {
                // 串行编译
                FragmentResult fragResult = compileFragment(fragment);
                result.fragmentResults.push_back(fragResult);
                processedFragments++;
            }
        }
        
        // 等待并行编译完成
        if (config.enableParallelCompilation) {
            for (auto& future : futures) {
                FragmentResult fragResult = future.get();
                result.fragmentResults.push_back(fragResult);
                processedFragments++;
            }
        }
        
        // 3. 合并编译结果
        result = mergeResults(result.fragmentResults);
        result.success = !result.hasErrors();
        
    } catch (const std::exception& e) {
        result.errors.push_back("Compilation Exception: " + String(e.what()));
        result.success = false;
    }
    
    return result;
}

FragmentResult CompilerDispatcher::compileFragment(const ScanResult& fragment) {
    FragmentResult result;
    result.fragmentType = fragment.type;
    result.success = false;
    
    try {
        if (fragment.type == "CHTL") {
            result = compileCHTLFragment(fragment);
        } else if (fragment.type == "CHTL_JS") {
            result = compileCHTLJSFragment(fragment);
        } else if (fragment.type == "PURE_JS") {
            result = compilePureJSFragment(fragment);
        } else if (fragment.type == "CSS") {
            result = compileCSSFragment(fragment);
        } else if (fragment.type == "HTML") {
            result = compileHTMLFragment(fragment);
        } else {
            result.errors.push_back("Unknown fragment type: " + fragment.type);
        }
        
    } catch (const std::exception& e) {
        result.errors.push_back("Fragment compilation error: " + String(e.what()));
    }
    
    return result;
}

FragmentResult CompilerDispatcher::compileCHTLFragment(const ScanResult& fragment) {
    FragmentResult result;
    result.fragmentType = "CHTL";
    
    try {
        // 创建CHTL词法分析器
        auto lexer = std::make_unique<Lexer>(fragment.content);
        
        // 创建CHTL解析器
        auto parser = std::make_unique<Parser>(std::move(lexer));
        
        // 解析AST
        auto ast = parser->parse();
        
        if (parser->hasErrors()) {
            for (const auto& error : parser->getErrors()) {
                result.errors.push_back("CHTL Parse Error: " + error.toString());
            }
            return result;
        }
        
        // 创建CHTL生成器
        auto generator = std::make_unique<CHTLGenerator>();
        
        // 生成输出
        result.htmlOutput = generator->generateHTML(ast.get());
        result.cssOutput = generator->generateCSS(ast.get());
        result.jsOutput = generator->generateJavaScript(ast.get());
        
        if (generator->hasErrors()) {
            for (const auto& error : generator->getErrors()) {
                result.errors.push_back("CHTL Generation Error: " + error);
            }
        }
        
        result.success = result.errors.empty();
        
    } catch (const std::exception& e) {
        result.errors.push_back("CHTL Compilation Error: " + String(e.what()));
    }
    
    return result;
}

FragmentResult CompilerDispatcher::compileCHTLJSFragment(const ScanResult& fragment) {
    FragmentResult result;
    result.fragmentType = "CHTL_JS";
    
    try {
        // 创建CHTL JS词法分析器
        auto lexer = std::make_unique<CHTLJSLexer>(fragment.content);
        
        // 创建CHTL JS解析器
        auto parser = std::make_unique<CHTLJSParser>(std::move(lexer));
        
        // 解析AST
        auto ast = parser->parse();
        
        if (parser->hasErrors()) {
            for (const auto& error : parser->getErrors()) {
                result.errors.push_back("CHTL JS Parse Error: " + error.toString());
            }
            return result;
        }
        
        // 创建CHTL JS生成器
        auto generator = std::make_unique<CHTLJSGenerator>();
        
        // 应用选择器自动化
        generator->enableSelectorAutomation(config.enableSelectorAutomation);
        
        // 生成JavaScript输出
        result.jsOutput = generator->generateJavaScript(ast.get());
        
        // 生成CSS（如果有选择器自动化）
        if (config.enableSelectorAutomation) {
            result.cssOutput = generator->generateAutomatedCSS(ast.get());
        }
        
        if (generator->hasErrors()) {
            for (const auto& error : generator->getErrors()) {
                result.errors.push_back("CHTL JS Generation Error: " + error);
            }
        }
        
        result.success = result.errors.empty();
        
    } catch (const std::exception& e) {
        result.errors.push_back("CHTL JS Compilation Error: " + String(e.what()));
    }
    
    return result;
}

FragmentResult CompilerDispatcher::compileCSSFragment(const ScanResult& fragment) {
    FragmentResult result;
    result.fragmentType = "CSS";
    
    try {
        if (config.useANTLRForCSS) {
            // 使用ANTLR CSS编译器（未实现）
            result.cssOutput = fragment.content; // 暂时直接输出
        } else {
            // 使用内置CSS处理器
            result.cssOutput = processCSSContent(fragment.content);
        }
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.errors.push_back("CSS Compilation Error: " + String(e.what()));
    }
    
    return result;
}

FragmentResult CompilerDispatcher::compilePureJSFragment(const ScanResult& fragment) {
    FragmentResult result;
    result.fragmentType = "PURE_JS";
    
    try {
        if (config.useANTLRForJS) {
            // 使用ANTLR JavaScript编译器处理纯JS
            result.jsOutput = compilePureJSWithANTLR(fragment.content);
        } else {
            // 直接输出纯JavaScript，确保是完整的JS字符串
            result.jsOutput = validateAndCleanJS(fragment.content);
        }
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.errors.push_back("Pure JavaScript Compilation Error: " + String(e.what()));
    }
    
    return result;
}

FragmentResult CompilerDispatcher::compileJSFragment(const ScanResult& fragment) {
    FragmentResult result;
    result.fragmentType = "JAVASCRIPT";
    
    try {
        if (config.useANTLRForJS) {
            // 使用ANTLR JavaScript编译器（未实现）
            result.jsOutput = fragment.content; // 暂时直接输出
        } else {
            // 使用内置JS处理器
            result.jsOutput = processJSContent(fragment.content);
        }
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.errors.push_back("JavaScript Compilation Error: " + String(e.what()));
    }
    
    return result;
}

FragmentResult CompilerDispatcher::compileHTMLFragment(const ScanResult& fragment) {
    FragmentResult result;
    result.fragmentType = "HTML";
    
    try {
        // HTML片段直接输出
        result.htmlOutput = fragment.content;
        result.success = true;
        
    } catch (const std::exception& e) {
        result.errors.push_back("HTML Processing Error: " + String(e.what()));
    }
    
    return result;
}

CompilationResult CompilerDispatcher::mergeResults(const std::vector<FragmentResult>& fragmentResults) {
    CompilationResult mergedResult;
    
    String htmlOutput;
    String cssOutput;
    String jsOutput;
    StringVector allErrors;
    StringVector allWarnings;
    
    // 合并所有fragment的输出
    for (const auto& fragResult : fragmentResults) {
        if (!fragResult.htmlOutput.empty()) {
            htmlOutput += fragResult.htmlOutput + "\n";
        }
        
        if (!fragResult.cssOutput.empty()) {
            cssOutput += fragResult.cssOutput + "\n";
        }
        
        if (!fragResult.jsOutput.empty()) {
            jsOutput += fragResult.jsOutput + "\n";
        }
        
        // 合并错误和警告
        allErrors.insert(allErrors.end(), fragResult.errors.begin(), fragResult.errors.end());
        allWarnings.insert(allWarnings.end(), fragResult.warnings.begin(), fragResult.warnings.end());
    }
    
    // 后处理和优化
    if (config.enableOptimization) {
        htmlOutput = optimizeHTML(htmlOutput);
        cssOutput = optimizeCSS(cssOutput);
        jsOutput = optimizeJS(jsOutput);
    }
    
    // 设置最终结果
    mergedResult.htmlOutput = htmlOutput;
    mergedResult.cssOutput = cssOutput;
    mergedResult.jsOutput = jsOutput;
    mergedResult.errors = allErrors;
    mergedResult.warnings = allWarnings;
    mergedResult.fragmentResults = fragmentResults;
    mergedResult.success = allErrors.empty();
    
    return mergedResult;
}

void CompilerDispatcher::initializeCompilers() {
    // 初始化各个编译器
    if (config.enableCHTLCompiler) {
        // CHTL编译器已集成到dispatch方法中
    }
    
    if (config.enableCHTLJSCompiler) {
        // CHTL JS编译器已集成到dispatch方法中
    }
    
    if (config.useANTLRForCSS) {
        // 初始化ANTLR CSS编译器（待实现）
    }
    
    if (config.useANTLRForJS) {
        // 初始化ANTLR JavaScript编译器（待实现）
    }
}

void CompilerDispatcher::cleanup() {
    // 清理资源
    totalFragments = 0;
    processedFragments = 0;
}

String CompilerDispatcher::processCSSContent(const String& content) {
    // 基础CSS处理
    String processed = content;
    
    // 移除多余空白
    std::regex whitespaceRegex(R"(\s+)");
    processed = std::regex_replace(processed, whitespaceRegex, " ");
    
    // 其他CSS处理逻辑
    return processed;
}

String CompilerDispatcher::processJSContent(const String& content) {
    // 基础JavaScript处理
    String processed = content;
    
    // 移除多余空白
    std::regex whitespaceRegex(R"(\s+)");
    processed = std::regex_replace(processed, whitespaceRegex, " ");
    
    // 其他JS处理逻辑
    return processed;
}

String CompilerDispatcher::optimizeHTML(const String& html) {
    if (!config.enableOptimization) return html;
    
    String optimized = html;
    
    // 移除HTML注释
    std::regex commentRegex(R"(<!--.*?-->)");
    optimized = std::regex_replace(optimized, commentRegex, "");
    
    // 压缩空白
    std::regex whitespaceRegex(R"(\s+)");
    optimized = std::regex_replace(optimized, whitespaceRegex, " ");
    
    return optimized;
}

String CompilerDispatcher::optimizeCSS(const String& css) {
    if (!config.enableOptimization) return css;
    
    String optimized = css;
    
    // 移除CSS注释
    std::regex commentRegex(R"(/\*.*?\*/)");
    optimized = std::regex_replace(optimized, commentRegex, "");
    
    // 压缩空白
    std::regex whitespaceRegex(R"(\s+)");
    optimized = std::regex_replace(optimized, whitespaceRegex, " ");
    
    // 移除分号前的空格
    std::regex semicolonRegex(R"(\s*;\s*)");
    optimized = std::regex_replace(optimized, semicolonRegex, ";");
    
    return optimized;
}

String CompilerDispatcher::optimizeJS(const String& js) {
    if (!config.enableOptimization) return js;
    
    String optimized = js;
    
    // 移除单行注释
    std::regex commentRegex(R"(//.*$)", std::regex_constants::multiline);
    optimized = std::regex_replace(optimized, commentRegex, "");
    
    // 移除多行注释
    std::regex multiCommentRegex(R"(/\*.*?\*/)");
    optimized = std::regex_replace(optimized, multiCommentRegex, "");
    
    // 压缩空白
    std::regex whitespaceRegex(R"(\s+)");
    optimized = std::regex_replace(optimized, whitespaceRegex, " ");
    
    return optimized;
}

double CompilerDispatcher::getProgress() const {
    if (totalFragments == 0) return 0.0;
    return static_cast<double>(processedFragments) / totalFragments;
}

bool CompilerDispatcher::isCompiling() const {
    return processedFragments < totalFragments;
}

void CompilerDispatcher::setConfig(const CompilerConfig& newConfig) {
    config = newConfig;
}

CompilerConfig CompilerDispatcher::getConfig() const {
    return config;
}

CompilerStats CompilerDispatcher::getStats() const {
    CompilerStats stats;
    stats.totalFragments = totalFragments;
    stats.processedFragments = processedFragments;
    stats.progress = getProgress();
    stats.isCompiling = isCompiling();
    return stats;
}

// CompilationResult实现
bool CompilationResult::hasErrors() const {
    return !errors.empty();
}

bool CompilationResult::hasWarnings() const {
    return !warnings.empty();
}

String CompilationResult::getFullHTML() const {
    String fullHTML = "<!DOCTYPE html>\n<html>\n<head>\n";
    
    if (!cssOutput.empty()) {
        fullHTML += "<style>\n" + cssOutput + "\n</style>\n";
    }
    
    fullHTML += "</head>\n<body>\n";
    fullHTML += htmlOutput;
    fullHTML += "\n";
    
    if (!jsOutput.empty()) {
        fullHTML += "<script>\n" + jsOutput + "\n</script>\n";
    }
    
    fullHTML += "</body>\n</html>";
    return fullHTML;
}

String CompilerDispatcher::compilePureJSWithANTLR(const String& jsContent) {
    // 使用ANTLR编译纯JavaScript
    // 这里应该调用ANTLR JavaScript解析器
    // 暂时返回清理后的内容
    return validateAndCleanJS(jsContent);
}

String CompilerDispatcher::validateAndCleanJS(const String& jsContent) {
    String cleaned = jsContent;
    
    // 移除多余的空白字符但保持代码结构
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s+$)"), ""); // 移除行尾空白
    cleaned = std::regex_replace(cleaned, std::regex(R"(^\s+)"), ""); // 移除行首空白
    
    // 确保语句以分号结尾（如果需要）
    if (!cleaned.empty() && cleaned.back() != ';' && cleaned.back() != '}') {
        cleaned += ";";
    }
    
    // 验证基本的JavaScript语法完整性
    int braceCount = 0;
    int parenCount = 0;
    bool inString = false;
    char stringChar = 0;
    
    for (size_t i = 0; i < cleaned.length(); ++i) {
        char c = cleaned[i];
        
        if (!inString && (c == '"' || c == '\'')) {
            inString = true;
            stringChar = c;
        } else if (inString && c == stringChar && (i == 0 || cleaned[i-1] != '\\')) {
            inString = false;
        } else if (!inString) {
            if (c == '{') braceCount++;
            else if (c == '}') braceCount--;
            else if (c == '(') parenCount++;
            else if (c == ')') parenCount--;
        }
    }
    
    // 如果括号不匹配，添加注释说明
    if (braceCount != 0 || parenCount != 0) {
        cleaned = "/* Warning: Unmatched braces or parentheses detected */\n" + cleaned;
    }
    
    return cleaned;
}

String CompilationResult::getErrorSummary() const {
    if (errors.empty()) return "No errors";
    
    String summary = "Compilation Errors (" + std::to_string(errors.size()) + "):\n";
    for (size_t i = 0; i < errors.size(); ++i) {
        summary += std::to_string(i + 1) + ". " + errors[i] + "\n";
    }
    return summary;
}

String CompilationResult::getWarningSummary() const {
    if (warnings.empty()) return "No warnings";
    
    String summary = "Compilation Warnings (" + std::to_string(warnings.size()) + "):\n";
    for (size_t i = 0; i < warnings.size(); ++i) {
        summary += std::to_string(i + 1) + ". " + warnings[i] + "\n";
    }
    return summary;
}

// ANTLR编译器管理方法
void CompilerDispatcher::setANTLRJavaScriptWrapper(std::unique_ptr<ANTLR::ANTLRJavaScriptWrapper> wrapper) {
    antlrJSWrapper = std::move(wrapper);
}

void CompilerDispatcher::setANTLRCSSWrapper(std::unique_ptr<ANTLR::ANTLRCSSWrapper> wrapper) {
    antlrCSSWrapper = std::move(wrapper);
}

bool CompilerDispatcher::isANTLREnabled() const {
    return (antlrJSWrapper != nullptr && antlrCSSWrapper != nullptr);
}

// ANTLR编译方法
CompilationResult CompilerDispatcher::compileJavaScriptWithANTLR(const String& jsCode) {
    CompilationResult result;
    result.success = false;
    
    if (!antlrJSWrapper) {
        result.errors.push_back("ANTLR JavaScript wrapper not initialized");
        return result;
    }
    
    try {
        auto parseResult = antlrJSWrapper->parseJavaScript(jsCode);
        
        if (parseResult.success) {
            result.javascript = parseResult.cleanedJavaScript;
            result.success = true;
            
            // 添加特性检测信息到warnings（作为信息提示）
            if (parseResult.hasModules) {
                result.warnings.push_back("JavaScript contains ES6 modules");
            }
            if (parseResult.hasArrowFunctions) {
                result.warnings.push_back("JavaScript contains arrow functions");
            }
            if (parseResult.hasAsyncAwait) {
                result.warnings.push_back("JavaScript contains async/await");
            }
            if (parseResult.hasClasses) {
                result.warnings.push_back("JavaScript contains ES6 classes");
            }
        } else {
            result.errors.push_back("ANTLR JavaScript parsing failed: " + parseResult.errorMessage);
            for (const auto& error : parseResult.syntaxErrors) {
                result.errors.push_back("Syntax error: " + error);
            }
        }
        
    } catch (const std::exception& e) {
        result.errors.push_back("ANTLR JavaScript compilation exception: " + String(e.what()));
    }
    
    return result;
}

CompilationResult CompilerDispatcher::compileCSSWithANTLR(const String& cssCode) {
    CompilationResult result;
    result.success = false;
    
    if (!antlrCSSWrapper) {
        result.errors.push_back("ANTLR CSS wrapper not initialized");
        return result;
    }
    
    try {
        auto parseResult = antlrCSSWrapper->parseCSS(cssCode);
        
        if (parseResult.success) {
            result.css = parseResult.cleanedCSS;
            result.success = true;
            
            // 添加特性检测信息到warnings
            if (parseResult.hasMediaQueries) {
                result.warnings.push_back("CSS contains media queries");
            }
            if (parseResult.hasKeyframes) {
                result.warnings.push_back("CSS contains keyframe animations");
            }
            if (parseResult.hasCustomProperties) {
                result.warnings.push_back("CSS contains custom properties (CSS variables)");
            }
            if (parseResult.hasGridLayout) {
                result.warnings.push_back("CSS contains CSS Grid layout");
            }
            if (parseResult.hasFlexbox) {
                result.warnings.push_back("CSS contains Flexbox layout");
            }
            if (parseResult.hasReferenceSelectors) {
                result.warnings.push_back("CSS contains CHTL reference selectors (&)");
            }
        } else {
            result.errors.push_back("ANTLR CSS parsing failed: " + parseResult.errorMessage);
            for (const auto& error : parseResult.syntaxErrors) {
                result.errors.push_back("Syntax error: " + error);
            }
        }
        
    } catch (const std::exception& e) {
        result.errors.push_back("ANTLR CSS compilation exception: " + String(e.what()));
    }
    
    return result;
}

bool CompilerDispatcher::validateJavaScriptSyntax(const String& jsCode) {
    if (!antlrJSWrapper) {
        return false;
    }
    
    try {
        return antlrJSWrapper->validateJavaScriptSyntax(jsCode);
    } catch (...) {
        return false;
    }
}

bool CompilerDispatcher::validateCSSSyntax(const String& cssCode) {
    if (!antlrCSSWrapper) {
        return false;
    }
    
    try {
        return antlrCSSWrapper->validateCSSSyntax(cssCode);
    } catch (...) {
        return false;
    }
}

} // namespace CHTL