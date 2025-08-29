#include "Dispatcher.h"
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include "../CHTLJS/CHTLJSParser/Parser.h"
#include "../CHTLJS/CHTLJSGenerator/Generator.h"
#include <algorithm>
#include <future>
#include <thread>

namespace CHTL {

// 修正后的编译器调度实现
CompilationResult CompilerDispatcher::compile(const String& sourceCode) {
    CompilationResult result;
    result.success = false;
    
    try {
        // 1. 使用修正后的统一扫描器进行代码切割
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
        
        // 2. 按类型分组处理fragments
        std::vector<ScanResult> chtlFragments;
        std::vector<ScanResult> chtljsFragments;
        std::vector<ScanResult> pureJSFragments;
        std::vector<ScanResult> cssFragments;
        std::vector<ScanResult> htmlFragments;
        
        for (const auto& fragment : fragments) {
            if (fragment.type == "CHTL") {
                chtlFragments.push_back(fragment);
            } else if (fragment.type == "CHTL_JS") {
                chtljsFragments.push_back(fragment);
            } else if (fragment.type == "PURE_JS") {
                pureJSFragments.push_back(fragment);
            } else if (fragment.type == "CSS") {
                cssFragments.push_back(fragment);
            } else if (fragment.type == "HTML") {
                htmlFragments.push_back(fragment);
            }
        }
        
        // 3. 分别编译各类型的fragments
        std::vector<std::future<FragmentResult>> futures;
        
        if (config.enableParallelCompilation) {
            // 并行编译各类型
            if (!chtlFragments.empty()) {
                futures.push_back(std::async(std::launch::async, 
                    [this, chtlFragments]() { return compileCHTLFragments(chtlFragments); }));
            }
            
            if (!chtljsFragments.empty()) {
                futures.push_back(std::async(std::launch::async, 
                    [this, chtljsFragments]() { return compileCHTLJSFragments(chtljsFragments); }));
            }
            
            if (!pureJSFragments.empty()) {
                futures.push_back(std::async(std::launch::async, 
                    [this, pureJSFragments]() { return compilePureJSFragments(pureJSFragments); }));
            }
            
            if (!cssFragments.empty()) {
                futures.push_back(std::async(std::launch::async, 
                    [this, cssFragments]() { return compileCSSFragments(cssFragments); }));
            }
            
            if (!htmlFragments.empty()) {
                futures.push_back(std::async(std::launch::async, 
                    [this, htmlFragments]() { return compileHTMLFragments(htmlFragments); }));
            }
            
            // 等待所有编译完成
            for (auto& future : futures) {
                FragmentResult fragResult = future.get();
                result.fragmentResults.push_back(fragResult);
                processedFragments++;
            }
        } else {
            // 串行编译
            if (!chtlFragments.empty()) {
                result.fragmentResults.push_back(compileCHTLFragments(chtlFragments));
            }
            if (!chtljsFragments.empty()) {
                result.fragmentResults.push_back(compileCHTLJSFragments(chtljsFragments));
            }
            if (!pureJSFragments.empty()) {
                result.fragmentResults.push_back(compilePureJSFragments(pureJSFragments));
            }
            if (!cssFragments.empty()) {
                result.fragmentResults.push_back(compileCSSFragments(cssFragments));
            }
            if (!htmlFragments.empty()) {
                result.fragmentResults.push_back(compileHTMLFragments(htmlFragments));
            }
        }
        
        // 4. 合并编译结果
        result = mergeResults(result.fragmentResults);
        result.success = !result.hasErrors();
        
    } catch (const std::exception& e) {
        result.errors.push_back("Compilation Exception: " + String(e.what()));
        result.success = false;
    }
    
    return result;
}

// 修正后的CHTL编译
FragmentResult CompilerDispatcher::compileCHTLFragments(const std::vector<ScanResult>& fragments) {
    FragmentResult result;
    result.fragmentType = "CHTL";
    
    try {
        // 合并所有CHTL片段内容
        String combinedCHTL;
        for (const auto& fragment : fragments) {
            combinedCHTL += fragment.content + "\n";
        }
        
        // 创建CHTL词法分析器
        auto lexer = std::make_unique<Lexer>(combinedCHTL);
        
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

// 修正后的CHTL JS编译
FragmentResult CompilerDispatcher::compileCHTLJSFragments(const std::vector<ScanResult>& fragments) {
    FragmentResult result;
    result.fragmentType = "CHTL_JS";
    
    try {
        // 合并所有CHTL JS片段内容
        String combinedCHTLJS;
        for (const auto& fragment : fragments) {
            combinedCHTLJS += fragment.content + "\n";
        }
        
        // 创建CHTL JS词法分析器
        auto lexer = std::make_unique<CHTLJSLexer>(combinedCHTLJS);
        
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
        
        // 启用选择器自动化
        generator->enableSelectorAutomation(config.enableSelectorAutomation);
        
        // 生成JavaScript输出
        result.jsOutput = generator->generateJavaScript(ast.get());
        
        // 生成自动化CSS（如果有选择器自动化）
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

// 新增：纯JavaScript编译处理
FragmentResult CompilerDispatcher::compilePureJSFragments(const std::vector<ScanResult>& fragments) {
    FragmentResult result;
    result.fragmentType = "PURE_JS";
    
    try {
        // 合并所有纯JS片段内容
        String combinedJS;
        for (const auto& fragment : fragments) {
            combinedJS += fragment.content + "\n";
        }
        
        if (config.useANTLRForJS) {
            // 使用ANTLR JavaScript编译器（未实现）
            result.jsOutput = combinedJS; // 暂时直接输出
        } else {
            // 使用内置JS处理器
            result.jsOutput = processJSContent(combinedJS);
        }
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.errors.push_back("Pure JavaScript Compilation Error: " + String(e.what()));
    }
    
    return result;
}

FragmentResult CompilerDispatcher::compileCSSFragments(const std::vector<ScanResult>& fragments) {
    FragmentResult result;
    result.fragmentType = "CSS";
    
    try {
        // 合并所有CSS片段内容
        String combinedCSS;
        for (const auto& fragment : fragments) {
            combinedCSS += fragment.content + "\n";
        }
        
        if (config.useANTLRForCSS) {
            // 使用ANTLR CSS编译器（未实现）
            result.cssOutput = combinedCSS; // 暂时直接输出
        } else {
            // 使用内置CSS处理器
            result.cssOutput = processCSSContent(combinedCSS);
        }
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.errors.push_back("CSS Compilation Error: " + String(e.what()));
    }
    
    return result;
}

FragmentResult CompilerDispatcher::compileHTMLFragments(const std::vector<ScanResult>& fragments) {
    FragmentResult result;
    result.fragmentType = "HTML";
    
    try {
        // 合并所有HTML片段内容
        String combinedHTML;
        for (const auto& fragment : fragments) {
            combinedHTML += fragment.content + "\n";
        }
        
        // HTML片段直接输出
        result.htmlOutput = combinedHTML;
        result.success = true;
        
    } catch (const std::exception& e) {
        result.errors.push_back("HTML Processing Error: " + String(e.what()));
    }
    
    return result;
}

// 修正后的结果合并
CompilationResult CompilerDispatcher::mergeResults(const std::vector<FragmentResult>& fragmentResults) {
    CompilationResult mergedResult;
    
    String htmlOutput;
    String cssOutput;
    String jsOutput;
    StringVector allErrors;
    StringVector allWarnings;
    
    // 分别收集各类型的输出
    String chtlHTML, chtlCSS;
    String chtljsJS, chtljsCSS;
    String pureJS;
    String globalCSS;
    String rawHTML;
    
    for (const auto& fragResult : fragmentResults) {
        // 收集错误和警告
        allErrors.insert(allErrors.end(), fragResult.errors.begin(), fragResult.errors.end());
        allWarnings.insert(allWarnings.end(), fragResult.warnings.begin(), fragResult.warnings.end());
        
        // 根据片段类型分别处理输出
        if (fragResult.fragmentType == "CHTL") {
            chtlHTML += fragResult.htmlOutput;
            chtlCSS += fragResult.cssOutput;
        } else if (fragResult.fragmentType == "CHTL_JS") {
            chtljsJS += fragResult.jsOutput;
            chtljsCSS += fragResult.cssOutput;
        } else if (fragResult.fragmentType == "PURE_JS") {
            pureJS += fragResult.jsOutput;
        } else if (fragResult.fragmentType == "CSS") {
            globalCSS += fragResult.cssOutput;
        } else if (fragResult.fragmentType == "HTML") {
            rawHTML += fragResult.htmlOutput;
        }
    }
    
    // 智能合并输出
    htmlOutput = mergeHTMLOutput(chtlHTML, rawHTML);
    cssOutput = mergeCSSOutput(chtlCSS, chtljsCSS, globalCSS);
    jsOutput = mergeJSOutput(chtljsJS, pureJS);
    
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

String CompilerDispatcher::mergeHTMLOutput(const String& chtlHTML, const String& rawHTML) {
    String merged;
    
    // 优先使用CHTL生成的HTML结构
    if (!chtlHTML.empty()) {
        merged += chtlHTML;
    }
    
    // 附加原始HTML片段
    if (!rawHTML.empty()) {
        merged += "\n" + rawHTML;
    }
    
    return merged;
}

String CompilerDispatcher::mergeCSSOutput(const String& chtlCSS, const String& chtljsCSS, const String& globalCSS) {
    String merged;
    
    // 按优先级合并CSS
    if (!globalCSS.empty()) {
        merged += "/* Global CSS */\n" + globalCSS + "\n";
    }
    
    if (!chtlCSS.empty()) {
        merged += "/* CHTL Generated CSS */\n" + chtlCSS + "\n";
    }
    
    if (!chtljsCSS.empty()) {
        merged += "/* CHTL JS Automated CSS */\n" + chtljsCSS + "\n";
    }
    
    return merged;
}

String CompilerDispatcher::mergeJSOutput(const String& chtljsJS, const String& pureJS) {
    String merged;
    
    // 首先添加CHTL JS生成的代码
    if (!chtljsJS.empty()) {
        merged += "// CHTL JS Generated Code\n";
        merged += "(function() {\n'use strict';\n";
        merged += chtljsJS;
        merged += "\n})();\n\n";
    }
    
    // 然后添加纯JavaScript代码
    if (!pureJS.empty()) {
        merged += "// Pure JavaScript Code\n";
        merged += "(function() {\n";
        merged += pureJS;
        merged += "\n})();\n";
    }
    
    return merged;
}

String CompilerDispatcher::processJSContent(const String& content) {
    // 增强的JavaScript处理
    String processed = content;
    
    // 移除多余空白但保持必要的格式
    std::regex excessiveWhitespace(R"(\n\s*\n\s*\n)");
    processed = std::regex_replace(processed, excessiveWhitespace, "\n\n");
    
    // 确保语句以分号结尾
    std::regex missingsemicolon(R"(([^;\s}])\s*\n)");
    processed = std::regex_replace(processed, missingsemicolon, "$1;\n");
    
    return processed;
}

String CompilerDispatcher::processCSSContent(const String& content) {
    // 增强的CSS处理
    String processed = content;
    
    // 移除多余空白
    std::regex excessiveWhitespace(R"(\s+)");
    processed = std::regex_replace(processed, excessiveWhitespace, " ");
    
    // 标准化CSS规则格式
    std::regex cssRule(R"(([^{]+)\s*\{\s*([^}]*)\s*\})");
    processed = std::regex_replace(processed, cssRule, "$1 {\n    $2\n}");
    
    return processed;
}

} // namespace CHTL