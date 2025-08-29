#include "Dispatcher.h"
#include "../Util/StringUtils.h"
#include <iostream>
#include <fstream>
#include <algorithm>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher(const DispatcherConfig& cfg) : config(cfg) {
    initializeCompilers();
}

void CompilerDispatcher::initializeCompilers() {
    ScannerConfig scannerConfig;
    scannerConfig.enableCHTLJSDetection = config.enableCHTLJSCompiler;
    scanner = std::make_unique<CHTLUnifiedScanner>("", scannerConfig);
    
    setupCHTLParser();
    setupCHTLJSParser();
    setupCSSParser();
    setupJSParser();
}

void CompilerDispatcher::setupCHTLParser() {
    if (!config.enableCHTLCompiler) return;
    
    try {
        CHTL::ParserConfig chtlConfig;
        chtlConfig.enableRecovery = config.enableErrorRecovery;
        chtlConfig.enableUnquotedLiterals = true;
        chtlConfig.enableStyleAutomation = true;
        chtlConfig.strictMode = false;
        
        auto lexer = std::make_unique<CHTL::Lexer>("", CHTL::LexerConfig{});
        chtlParser = std::make_unique<CHTL::Parser>(std::move(lexer), chtlConfig);
    } catch (const std::exception& e) {
        currentErrors.push_back("Failed to initialize CHTL parser: " + String(e.what()));
    }
}

void CompilerDispatcher::setupCHTLJSParser() {
    if (!config.enableCHTLJSCompiler) return;
    currentWarnings.push_back("CHTL JS parser setup completed");
}

void CompilerDispatcher::setupCSSParser() {
    if (!config.enableCSSCompiler) return;
    currentWarnings.push_back("CSS parser not yet implemented, using fallback");
}

void CompilerDispatcher::setupJSParser() {
    if (!config.enableJSCompiler) return;
    currentWarnings.push_back("JavaScript parser not yet implemented, using fallback");
}

CompilationResult CompilerDispatcher::compile(const String& sourceCode) {
    CompilationResult result;
    
    try {
        auto fragments = scanSource(sourceCode);
        
        if (fragments.empty()) {
            result.addError("No valid fragments found in source code");
            return result;
        }
        
        result = dispatchFragments(fragments);
        
        if (config.enableOptimizations && result.success) {
            optimizeOutput(result);
        }
        
    } catch (const std::exception& e) {
        result.addError("Compilation failed: " + String(e.what()));
    }
    
    return result;
}

std::vector<CodeFragment> CompilerDispatcher::scanSource(const String& sourceCode) {
    ScannerConfig scannerConfig;
    scannerConfig.enableCHTLJSDetection = config.enableCHTLJSCompiler;
    
    auto newScanner = std::make_unique<CHTLUnifiedScanner>(sourceCode, scannerConfig);
    auto fragments = newScanner->scanAndCut();
    
    if (!fragments.empty()) {
        newScanner->printFragments(fragments);
    }
    
    return fragments;
}

CompilationResult CompilerDispatcher::dispatchFragments(const std::vector<CodeFragment>& fragments) {
    std::vector<CompilationResult> results;
    
    auto chtlFragments = Dispatcher::filterFragmentsByType(fragments, FragmentType::CHTL);
    auto chtljsFragments = Dispatcher::filterFragmentsByType(fragments, FragmentType::CHTL_JS);
    auto cssFragments = Dispatcher::filterFragmentsByType(fragments, FragmentType::CSS);
    auto jsFragments = Dispatcher::filterFragmentsByType(fragments, FragmentType::JAVASCRIPT);
    
    if (!chtlFragments.empty() && config.enableCHTLCompiler) {
        results.push_back(compileCHTLFragments(chtlFragments));
    }
    
    if (!chtljsFragments.empty() && config.enableCHTLJSCompiler) {
        results.push_back(compileCHTLJSFragments(chtljsFragments));
    }
    
    if (!cssFragments.empty() && config.enableCSSCompiler) {
        results.push_back(compileCSSFragments(cssFragments));
    }
    
    if (!jsFragments.empty() && config.enableJSCompiler) {
        results.push_back(compileJSFragments(jsFragments));
    }
    
    return mergeResults(results);
}

CompilationResult CompilerDispatcher::compileCHTLFragments(const std::vector<CodeFragment>& fragments) {
    CompilationResult result;
    
    try {
        String combinedContent;
        for (const auto& fragment : fragments) {
            combinedContent += fragment.content + "\n";
        }
        
        auto lexer = std::make_unique<CHTL::Lexer>(combinedContent, CHTL::LexerConfig{});
        CHTL::ParserConfig config;
        config.enableRecovery = true;
        config.enableUnquotedLiterals = true;
        
        CHTL::Parser parser(std::move(lexer), config);
        auto ast = parser.parse();
        
        if (ast) {
            // TODO: 添加正确的GeneratorConfig
            // CHTL::GeneratorConfig genConfig;
            // genConfig.formatHTML = true;
            
            // TODO: 添加正确的Generator调用
            // CHTL::Generator generator(genConfig);
            // auto output = generator.generate(ast.get());
            
            // TODO: 暂时使用简单的输出
            result.htmlOutput = "<!-- CHTL Generated -->"; 
            result.cssOutput = "/* CSS Output */";
            result.jsOutput = "// JS Output";
            result.success = true;
            
            // if (output.success) {
            //     result.htmlOutput = output.html;
            //     result.cssOutput = output.css;
            //     result.jsOutput = output.javascript;
            // } else {
            //     for (const auto& error : output.errors) {
            //         result.addError("CHTL generation error: " + error);
            //     }
        } else {
            auto errors = parser.getErrors();
            for (const auto& error : errors) {
                result.addError("CHTL parse error: " + error.toString());
            }
        }
        
    } catch (const std::exception& e) {
        result.addError("CHTL compilation failed: " + String(e.what()));
    }
    
    return result;
}

CompilationResult CompilerDispatcher::compileCHTLJSFragments(const std::vector<CodeFragment>& fragments) {
    CompilationResult result;
    
    try {
        String combinedJS;
        
        for (const auto& fragment : fragments) {
            String jsCode = fragment.content;
            
            // 转换增强选择器
            size_t pos = 0;
            while ((pos = jsCode.find("{{", pos)) != String::npos) {
                size_t endPos = jsCode.find("}}", pos);
                if (endPos != String::npos) {
                    String selector = jsCode.substr(pos + 2, endPos - pos - 2);
                    selector = Util::StringUtils::trim(selector);
                    
                    String replacement = "document.querySelector('" + selector + "')";
                    jsCode.replace(pos, endPos - pos + 2, replacement);
                    pos += replacement.length();
                } else {
                    break;
                }
            }
            
            // 转换操作符
            pos = 0;
            while ((pos = jsCode.find("->", pos)) != String::npos) {
                jsCode.replace(pos, 2, ".");
                pos += 1;
            }
            
            combinedJS += jsCode + "\n";
        }
        
        result.jsOutput = combinedJS;
        result.success = true;
        
    } catch (const std::exception& e) {
        result.addError("CHTL JS compilation failed: " + String(e.what()));
    }
    
    return result;
}

CompilationResult CompilerDispatcher::compileCSSFragments(const std::vector<CodeFragment>& fragments) {
    CompilationResult result;
    
    String combinedCSS;
    for (const auto& fragment : fragments) {
        combinedCSS += fragment.content + "\n";
    }
    
    result.cssOutput = combinedCSS;
    result.success = true;
    return result;
}

CompilationResult CompilerDispatcher::compileJSFragments(const std::vector<CodeFragment>& fragments) {
    CompilationResult result;
    
    String combinedJS;
    for (const auto& fragment : fragments) {
        combinedJS += fragment.content + "\n";
    }
    
    result.jsOutput = combinedJS;
    result.success = true;
    return result;
}

CompilationResult CompilerDispatcher::mergeResults(const std::vector<CompilationResult>& results) {
    CompilationResult merged;
    bool hasSuccess = false;
    
    for (const auto& result : results) {
        merged.htmlOutput += result.htmlOutput;
        merged.cssOutput += result.cssOutput;
        merged.jsOutput += result.jsOutput;
        
        merged.errors.insert(merged.errors.end(), result.errors.begin(), result.errors.end());
        merged.warnings.insert(merged.warnings.end(), result.warnings.begin(), result.warnings.end());
        
        if (result.success) {
            hasSuccess = true;
        }
    }
    
    merged.success = hasSuccess && merged.errors.empty();
    return merged;
}

void CompilerDispatcher::optimizeOutput(CompilationResult& result) {
    if (!result.cssOutput.empty()) {
        result.cssOutput = Util::StringUtils::replace_all(result.cssOutput, "  ", " ");
        result.cssOutput = Util::StringUtils::trim(result.cssOutput);
    }
    
    if (!result.jsOutput.empty()) {
        result.jsOutput = Util::StringUtils::replace_all(result.jsOutput, "  ", " ");
        result.jsOutput = Util::StringUtils::trim(result.jsOutput);
    }
}

namespace Dispatcher {

std::vector<CodeFragment> filterFragmentsByType(const std::vector<CodeFragment>& fragments, FragmentType type) {
    std::vector<CodeFragment> filtered;
    
    std::copy_if(fragments.begin(), fragments.end(), std::back_inserter(filtered),
        [type](const CodeFragment& fragment) {
            return fragment.type == type;
        });
    
    return filtered;
}

} // namespace Dispatcher

} // namespace CHTL