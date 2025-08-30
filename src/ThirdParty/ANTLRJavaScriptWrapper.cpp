#include "ANTLRJavaScriptWrapper.h"
#include <iostream>
#include <sstream>
#include <regex>

namespace CHTL {
namespace ANTLR {

    // JavaScript错误监听器实现
    JavaScriptErrorListener::JavaScriptErrorListener(ANTLRJavaScriptWrapper::ParseResult* result) 
        : parseResult(result) {}

    void JavaScriptErrorListener::syntaxError(antlr4::Recognizer* recognizer,
                                            antlr4::Token* offendingSymbol,
                                            size_t line,
                                            size_t charPositionInLine,
                                            const std::string& msg,
                                            std::exception_ptr e) {
        if (parseResult) {
            std::stringstream errorStream;
            errorStream << "Line " << line << ":" << charPositionInLine << " - " << msg;
            parseResult->syntaxErrors.push_back(errorStream.str());
            parseResult->success = false;
            parseResult->hasValidSyntax = false;
        }
    }

    // ANTLRJavaScriptWrapper实现
    ANTLRJavaScriptWrapper::ANTLRJavaScriptWrapper() {
        defaultConfig.strictMode = true;
        defaultConfig.useWindowsLineEndings = true;
        defaultConfig.handleUTF8BOM = true;
    }

    ANTLRJavaScriptWrapper::~ANTLRJavaScriptWrapper() {
        reset();
    }

    ANTLRJavaScriptWrapper::ParseResult ANTLRJavaScriptWrapper::parseJavaScript(
        const String& jsCode, const ParseConfig& config) {
        
        ParseResult result;
        result.clear();

        try {
            // 预处理代码
            String processedCode = jsCode;
            if (config.handleUTF8BOM) {
                processedCode = handleUTF8BOM(processedCode);
            }
            if (config.useWindowsLineEndings) {
                processedCode = normalizeLineEndings(processedCode, true);
            }

            // 设置解析器
            setupParser(processedCode);
            
            // 添加错误监听器
            auto errorListener = std::make_unique<JavaScriptErrorListener>(&result);
            parser->removeErrorListeners();
            parser->addErrorListener(errorListener.get());

            // 解析
            JavaScriptParser::ProgramContext* tree = parser->program();
            
            if (tree && result.syntaxErrors.empty()) {
                result.success = true;
                result.hasValidSyntax = true;
                
                if (config.extractFeatures) {
                    analyzeAST(tree, result);
                }
                
                if (config.cleanOutput) {
                    result.cleanedJavaScript = extractCleanCode(tree);
                }
            } else {
                result.success = false;
                if (!result.syntaxErrors.empty()) {
                    result.errorMessage = result.syntaxErrors[0];
                }
            }

        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = "ANTLR解析异常: " + String(e.what());
            lastError = result.errorMessage;
        }

        return result;
    }

    bool ANTLRJavaScriptWrapper::validateJavaScriptSyntax(const String& jsCode) {
        ParseResult result = parseJavaScript(jsCode, ParseConfig());
        return result.success && result.hasValidSyntax;
    }

    String ANTLRJavaScriptWrapper::cleanJavaScriptCode(const String& jsCode) {
        ParseConfig config;
        config.cleanOutput = true;
        config.extractFeatures = false;
        
        ParseResult result = parseJavaScript(jsCode, config);
        if (result.success) {
            return result.cleanedJavaScript;
        }
        
        // 如果解析失败，进行基本的清理
        String cleaned = jsCode;
        // 移除单行注释
        cleaned = std::regex_replace(cleaned, std::regex("//.*"), "");
        // 移除多行注释
        cleaned = std::regex_replace(cleaned, std::regex("/\\*[\\s\\S]*?\\*/"), "");
        // 规范化空白字符
        cleaned = std::regex_replace(cleaned, std::regex("\\s+"), " ");
        
        return cleaned;
    }

    ANTLRJavaScriptWrapper::ParseResult ANTLRJavaScriptWrapper::detectJavaScriptFeatures(const String& jsCode) {
        ParseConfig config;
        config.extractFeatures = true;
        config.cleanOutput = false;
        
        return parseJavaScript(jsCode, config);
    }

    String ANTLRJavaScriptWrapper::extractPureJavaScript(const String& mixedCode) {
        // 这个方法专门处理CHTL JS和纯JavaScript的混合代码
        // 移除CHTL JS特有的语法，保留纯JavaScript部分
        
        String cleaned = mixedCode;
        
        // 移除CHTL JS选择器语法 {{...}}
        cleaned = std::regex_replace(cleaned, std::regex("\\{\\{[^}]*\\}\\}"), "document.querySelector('')");
        
        // 移除CHTL JS箭头语法 ->
        cleaned = std::regex_replace(cleaned, std::regex("->\\s*listen"), ".addEventListener");
        cleaned = std::regex_replace(cleaned, std::regex("->\\s*delegate"), ".addEventListener");
        
        // 移除CHTL JS事件绑定操作符 &->
        cleaned = std::regex_replace(cleaned, std::regex("&->"), ".addEventListener('");
        
        // 移除CHTL JS特有的语法块
        cleaned = std::regex_replace(cleaned, std::regex("listen\\s*\\{[^}]*\\}"), "");
        cleaned = std::regex_replace(cleaned, std::regex("delegate\\s*\\{[^}]*\\}"), "");
        cleaned = std::regex_replace(cleaned, std::regex("animate\\s*\\{[^}]*\\}"), "");
        cleaned = std::regex_replace(cleaned, std::regex("vir\\s+\\w+\\s*="), "var ");
        
        // 清理多余的空白
        cleaned = std::regex_replace(cleaned, std::regex("\\s+"), " ");
        cleaned = std::regex_replace(cleaned, std::regex("^\\s+|\\s+$"), "");
        
        return cleaned;
    }

    void ANTLRJavaScriptWrapper::reset() {
        inputStream.reset();
        lexer.reset();
        tokens.reset();
        parser.reset();
        lastError.clear();
    }

    bool ANTLRJavaScriptWrapper::checkWindowsCompatibility() {
        // 检查ANTLR4运行时是否正确加载
        try {
            antlr4::ANTLRInputStream testStream("test");
            return true;
        } catch (...) {
            return false;
        }
    }

    void ANTLRJavaScriptWrapper::setupParser(const String& jsCode) {
        reset();
        
        inputStream = std::make_unique<antlr4::ANTLRInputStream>(jsCode);
        lexer = std::make_unique<JavaScriptLexer>(inputStream.get());
        tokens = std::make_unique<antlr4::CommonTokenStream>(lexer.get());
        parser = std::make_unique<JavaScriptParser>(tokens.get());
    }

    void ANTLRJavaScriptWrapper::collectErrors(ParseResult& result) {
        // 错误已经通过ErrorListener收集
    }

    void ANTLRJavaScriptWrapper::analyzeAST(JavaScriptParser::ProgramContext* tree, ParseResult& result) {
        if (!tree) return;

        // 简单的AST分析，检测JavaScript特性
        String treeText = tree->getText();
        
        // 检测模块语法
        result.hasModules = (treeText.find("import") != String::npos || 
                           treeText.find("export") != String::npos);
        
        // 检测箭头函数
        result.hasArrowFunctions = (treeText.find("=>") != String::npos);
        
        // 检测async/await
        result.hasAsyncAwait = (treeText.find("async") != String::npos || 
                              treeText.find("await") != String::npos);
        
        // 检测类
        result.hasClasses = (treeText.find("class") != String::npos);
        
        // 检测ES6特性
        result.hasES6Features = (result.hasArrowFunctions || result.hasClasses || 
                               treeText.find("let") != String::npos || 
                               treeText.find("const") != String::npos);
    }

    String ANTLRJavaScriptWrapper::extractCleanCode(JavaScriptParser::ProgramContext* tree) {
        if (!tree) return "";
        
        // 简单的代码提取，移除空白和注释
        String code = tree->getText();
        
        // 基本清理
        code = std::regex_replace(code, std::regex("\\s+"), " ");
        code = std::regex_replace(code, std::regex("^\\s+|\\s+$"), "");
        
        return code;
    }

    String ANTLRJavaScriptWrapper::normalizeLineEndings(const String& code, bool useWindows) {
        String result = code;
        if (useWindows) {
            // 转换为Windows行结束符
            result = std::regex_replace(result, std::regex("\r\n"), "\n");
            result = std::regex_replace(result, std::regex("\r"), "\n");
            result = std::regex_replace(result, std::regex("\n"), "\r\n");
        } else {
            // 转换为Unix行结束符
            result = std::regex_replace(result, std::regex("\r\n"), "\n");
            result = std::regex_replace(result, std::regex("\r"), "\n");
        }
        return result;
    }

    String ANTLRJavaScriptWrapper::handleUTF8BOM(const String& code) {
        // 移除UTF-8 BOM（如果存在）
        if (code.size() >= 3 && 
            static_cast<unsigned char>(code[0]) == 0xEF &&
            static_cast<unsigned char>(code[1]) == 0xBB &&
            static_cast<unsigned char>(code[2]) == 0xBF) {
            return code.substr(3);
        }
        return code;
    }

} // namespace ANTLR
} // namespace CHTL