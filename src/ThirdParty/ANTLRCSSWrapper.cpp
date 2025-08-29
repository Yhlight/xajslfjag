#include "ANTLRCSSWrapper.h"
#include <iostream>
#include <sstream>
#include <regex>

namespace CHTL {
namespace ANTLR {

    // CSS错误监听器实现
    CSSErrorListener::CSSErrorListener(ANTLRCSSWrapper::ParseResult* result) 
        : parseResult(result) {}

    void CSSErrorListener::syntaxError(antlr4::Recognizer* recognizer,
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

    // ANTLRCSSWrapper实现
    ANTLRCSSWrapper::ANTLRCSSWrapper() {
        defaultConfig.strictMode = true;
        defaultConfig.useWindowsLineEndings = true;
        defaultConfig.handleUTF8BOM = true;
        defaultConfig.processReferenceSelectors = true;
        defaultConfig.autoAddClasses = true;
        defaultConfig.autoAddIds = false;
    }

    ANTLRCSSWrapper::~ANTLRCSSWrapper() {
        reset();
    }

    ANTLRCSSWrapper::ParseResult ANTLRCSSWrapper::parseCSS(
        const String& cssCode, const ParseConfig& config) {
        
        ParseResult result;
        result.clear();

        try {
            // 预处理代码
            String processedCode = cssCode;
            if (config.handleUTF8BOM) {
                processedCode = handleUTF8BOM(processedCode);
            }
            if (config.useWindowsLineEndings) {
                processedCode = normalizeLineEndings(processedCode, true);
            }
            if (config.processReferenceSelectors) {
                processedCode = processReferenceSelectors(processedCode);
            }

            // 设置解析器
            setupParser(processedCode);
            
            // 添加错误监听器
            auto errorListener = std::make_unique<CSSErrorListener>(&result);
            parser->removeErrorListeners();
            parser->addErrorListener(errorListener.get());

            // 解析
            css3Parser::StylesheetContext* tree = parser->stylesheet();
            
            if (tree && result.syntaxErrors.empty()) {
                result.success = true;
                result.hasValidSyntax = true;
                
                if (config.extractFeatures) {
                    analyzeAST(tree, result);
                }
                
                if (config.cleanOutput) {
                    result.cleanedCSS = extractCleanCode(tree);
                }
            } else {
                result.success = false;
                if (!result.syntaxErrors.empty()) {
                    result.errorMessage = result.syntaxErrors[0];
                }
            }

        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = "ANTLR CSS解析异常: " + String(e.what());
            lastError = result.errorMessage;
        }

        return result;
    }

    bool ANTLRCSSWrapper::validateCSSSyntax(const String& cssCode) {
        ParseResult result = parseCSS(cssCode, ParseConfig());
        return result.success && result.hasValidSyntax;
    }

    String ANTLRCSSWrapper::cleanCSSCode(const String& cssCode) {
        ParseConfig config;
        config.cleanOutput = true;
        config.extractFeatures = false;
        
        ParseResult result = parseCSS(cssCode, config);
        if (result.success) {
            return result.cleanedCSS;
        }
        
        // 如果解析失败，进行基本的清理
        String cleaned = cssCode;
        // 移除注释
        cleaned = std::regex_replace(cleaned, std::regex("/\\*[\\s\\S]*?\\*/"), "");
        // 规范化空白字符
        cleaned = std::regex_replace(cleaned, std::regex("\\s+"), " ");
        // 移除多余的分号
        cleaned = std::regex_replace(cleaned, std::regex(";;+"), ";");
        
        return cleaned;
    }

    ANTLRCSSWrapper::ParseResult ANTLRCSSWrapper::detectCSSFeatures(const String& cssCode) {
        ParseConfig config;
        config.extractFeatures = true;
        config.cleanOutput = false;
        
        return parseCSS(cssCode, config);
    }

    String ANTLRCSSWrapper::processCHTLCSS(const String& cssCode) {
        // 处理CHTL CSS特殊语法
        String processed = cssCode;
        
        // 处理引用选择器 &
        processed = processReferenceSelectors(processed);
        
        return processed;
    }

    std::vector<String> ANTLRCSSWrapper::extractSelectors(const String& cssCode) {
        std::vector<String> selectors;
        
        ParseResult result = parseCSS(cssCode);
        if (result.success) {
            // 简单的选择器提取
            std::regex selectorRegex("([^{]+)\\s*\\{");
            std::sregex_iterator iter(cssCode.begin(), cssCode.end(), selectorRegex);
            std::sregex_iterator end;
            
            for (; iter != end; ++iter) {
                String selector = iter->str(1);
                // 清理空白
                selector = std::regex_replace(selector, std::regex("^\\s+|\\s+$"), "");
                if (!selector.empty()) {
                    selectors.push_back(selector);
                }
            }
        }
        
        return selectors;
    }

    void ANTLRCSSWrapper::reset() {
        inputStream.reset();
        lexer.reset();
        tokens.reset();
        parser.reset();
        lastError.clear();
    }

    bool ANTLRCSSWrapper::checkWindowsCompatibility() {
        // 检查ANTLR4运行时是否正确加载
        try {
            antlr4::ANTLRInputStream testStream("test");
            return true;
        } catch (...) {
            return false;
        }
    }

    void ANTLRCSSWrapper::setupParser(const String& cssCode) {
        reset();
        
        inputStream = std::make_unique<antlr4::ANTLRInputStream>(cssCode);
        lexer = std::make_unique<css3Lexer>(inputStream.get());
        tokens = std::make_unique<antlr4::CommonTokenStream>(lexer.get());
        parser = std::make_unique<css3Parser>(tokens.get());
    }

    void ANTLRCSSWrapper::collectErrors(ParseResult& result) {
        // 错误已经通过ErrorListener收集
    }

    void ANTLRCSSWrapper::analyzeAST(css3Parser::StylesheetContext* tree, ParseResult& result) {
        if (!tree) return;

        // 简单的AST分析，检测CSS特性
        String treeText = tree->getText();
        
        // 检测媒体查询
        result.hasMediaQueries = (treeText.find("@media") != String::npos);
        
        // 检测关键帧动画
        result.hasKeyframes = (treeText.find("@keyframes") != String::npos ||
                             treeText.find("@-webkit-keyframes") != String::npos);
        
        // 检测CSS自定义属性
        result.hasCustomProperties = (treeText.find("--") != String::npos);
        
        // 检测Grid布局
        result.hasGridLayout = (treeText.find("grid") != String::npos ||
                              treeText.find("grid-template") != String::npos);
        
        // 检测Flexbox
        result.hasFlexbox = (treeText.find("flex") != String::npos ||
                           treeText.find("display:flex") != String::npos);
        
        // 检测选择器
        result.hasSelectors = (treeText.find(".") != String::npos ||
                             treeText.find("#") != String::npos ||
                             treeText.find("[") != String::npos);
        
        // 检测CHTL特殊选择器
        result.hasCHTLSelectors = (treeText.find("{{") != String::npos);
        result.hasReferenceSelectors = (treeText.find("&") != String::npos);
    }

    String ANTLRCSSWrapper::extractCleanCode(css3Parser::StylesheetContext* tree) {
        if (!tree) return "";
        
        // 简单的代码提取，移除空白和注释
        String code = tree->getText();
        
        // 基本清理
        code = std::regex_replace(code, std::regex("\\s+"), " ");
        code = std::regex_replace(code, std::regex("^\\s+|\\s+$"), "");
        
        return code;
    }

    String ANTLRCSSWrapper::normalizeLineEndings(const String& code, bool useWindows) {
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

    String ANTLRCSSWrapper::handleUTF8BOM(const String& code) {
        // 移除UTF-8 BOM（如果存在）
        if (code.size() >= 3 && 
            static_cast<unsigned char>(code[0]) == 0xEF &&
            static_cast<unsigned char>(code[1]) == 0xBB &&
            static_cast<unsigned char>(code[2]) == 0xBF) {
            return code.substr(3);
        }
        return code;
    }

    String ANTLRCSSWrapper::processReferenceSelectors(const String& cssCode) {
        // 处理CHTL CSS引用选择器 &
        String processed = cssCode;
        
        // 这是一个简化的实现，实际应该根据上下文处理
        // 例如：在.container { & .item { } } 中，& 应该被替换为 .container
        
        // 这里先进行基本的处理
        // 实际实现应该更复杂，需要分析CSS结构
        
        return processed;
    }

} // namespace ANTLR
} // namespace CHTL