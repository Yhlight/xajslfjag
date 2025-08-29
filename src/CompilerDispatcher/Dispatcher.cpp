#include "Dispatcher.h"
#include "../Util/StringUtils.h"
#include "../CHTL/CHTLGenerator/Generator.h"
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
            
            // 使用完善的Generator
            // 配置生成器
            CHTL::Generator generator;
            auto output = generator.generate(ast.get());
            
            if (output.success) {
                result.htmlOutput = output.html;
                result.cssOutput = output.css;
                result.jsOutput = output.javascript;
                result.success = true;
            } else {
                for (const auto& error : output.errors) {
                    result.addError("CHTL generation error: " + error);
                }
            }
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

CompilationResult CompilerDispatcher::compileFragments(const std::vector<CodeFragment>& fragments) {
    CompilationResult result;
    
    try {
        std::ostringstream htmlOutput, cssOutput, jsOutput;
        
        for (const auto& fragment : fragments) {
            switch (fragment.type) {
                case FragmentType::CHTL: {
                    // 简化处理：直接输出片段内容
                    htmlOutput << fragment.content;
                    break;
                }
                
                case FragmentType::CSS: {
                    // 使用CSS解析器处理CSS片段
                    if (!cssParser) {
                        CSS::CSSParserConfig cssConfig;
                        cssParser = std::make_unique<CSS::Parser>(cssConfig);
                    }
                    
                    auto cssResult = cssParser->parse(fragment.content);
                    if (cssResult.success) {
                        cssOutput << cssResult.css;
                        if (!cssResult.css.empty() && !CHTL::Util::StringUtils::ends_with(cssResult.css, "\n")) {
                            cssOutput << "\n";
                        }
                    } else {
                        for (const auto& error : cssResult.errors) {
                            result.addError("CSS解析错误: " + error);
                        }
                        // 解析失败时仍然输出原始内容
                        cssOutput << fragment.content;
                    }
                    break;
                }
                
                case FragmentType::JAVASCRIPT: {
                    // 使用JS解析器处理JavaScript片段
                    if (!jsParser) {
                        JS::JSParserConfig jsConfig;
                        jsParser = std::make_unique<JS::Parser>(jsConfig);
                    }
                    
                    auto jsResult = jsParser->parse(fragment.content);
                    if (jsResult.success) {
                        jsOutput << jsResult.javascript;
                        if (!jsResult.javascript.empty() && !CHTL::Util::StringUtils::ends_with(jsResult.javascript, "\n")) {
                            jsOutput << "\n";
                        }
                    } else {
                        for (const auto& error : jsResult.errors) {
                            result.addError("JavaScript解析错误: " + error);
                        }
                        // 解析失败时仍然输出原始内容
                        jsOutput << fragment.content;
                    }
                    break;
                }
                
                case FragmentType::CHTL_JS: {
                    // CHTL JS需要通过增强解析器处理
                    std::vector<CodeFragment> chtljsFragments = {fragment};
                    auto chtljsResult = compileCHTLJSFragments(chtljsFragments);
                    if (chtljsResult.success) {
                        jsOutput << chtljsResult.jsOutput;
                    } else {
                        for (const auto& error : chtljsResult.errors) {
                            result.addError("CHTL JS片段编译错误: " + error);
                        }
                    }
                    break;
                }
                
                case FragmentType::HTML: {
                    // 纯文本作为HTML内容
                    htmlOutput << fragment.content;
                    break;
                }
                
                default:
                    result.addWarning("未知片段类型: " + std::to_string(static_cast<int>(fragment.type)));
                    break;
            }
        }
        
        result.htmlOutput = htmlOutput.str();
        result.cssOutput = cssOutput.str();
        result.jsOutput = jsOutput.str();
        result.success = result.errors.empty();
        
    } catch (const std::exception& e) {
        result.addError("片段编译过程中发生异常: " + String(e.what()));
    }
    
    return result;
}

} // namespace CHTL