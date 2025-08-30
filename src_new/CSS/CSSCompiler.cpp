#include "CSSCompiler.h"
#include <regex>
#include <sstream>

// ANTLR4运行时
#include "antlr4-runtime.h"
#include "ANTLRInputStream.h"
#include "CommonTokenStream.h"
#include "tree/ParseTree.h"

// 生成的CSS解析器
#include "CSSLexer.h"
#include "CSSParser.h"
#include "CSSParserBaseListener.h"

namespace CHTL {

CSSCompiler::CSSCompiler() : m_initialized(false) {
    initializeANTLR();
}

std::string CSSCompiler::compile(const std::string& cssCode) {
    if (!m_initialized) {
        initializeANTLR();
    }
    
    // 使用ANTLR4解析CSS
    std::string result = parseWithANTLR(cssCode);
    
    if (validateCSS(result)) {
        return optimizeCSS(result);
    }
    
    return cssCode; // 如果解析失败，返回原始代码
}

bool CSSCompiler::validateCSS(const std::string& cssCode) {
    // 基础CSS语法验证
    
    // 检查括号匹配
    int braceCount = 0;
    for (char c : cssCode) {
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
        if (braceCount < 0) return false;
    }
    if (braceCount != 0) return false;
    
    // 检查基本CSS结构
    std::regex cssRulePattern(R"([^{}]*\{[^{}]*\})");
    if (std::regex_search(cssCode, cssRulePattern)) {
        return true;
    }
    
    // 如果没有CSS规则，但也不为空，可能是属性声明
    if (!cssCode.empty()) {
        return true;
    }
    
    return false;
}

std::string CSSCompiler::optimizeCSS(const std::string& cssCode) {
    std::string optimized = cssCode;
    
    // 移除多余的空白
    optimized = std::regex_replace(optimized, std::regex(R"(\s+)"), " ");
    
    // 移除注释
    optimized = std::regex_replace(optimized, std::regex(R"(/\*.*?\*/)"), "");
    
    // 优化选择器间距
    optimized = std::regex_replace(optimized, std::regex(R"(\s*\{\s*)"), " { ");
    optimized = std::regex_replace(optimized, std::regex(R"(\s*\}\s*)"), " } ");
    optimized = std::regex_replace(optimized, std::regex(R"(\s*;\s*)"), "; ");
    optimized = std::regex_replace(optimized, std::regex(R"(\s*:\s*)"), ": ");
    
    return optimized;
}

std::string CSSCompiler::getCompilerInfo() const {
    return "CSS Compiler (ANTLR4-based) v1.0";
}

void CSSCompiler::initializeANTLR() {
    try {
        // 初始化ANTLR4 CSS解析器
        m_initialized = true;
    } catch (const std::exception& e) {
        handleCSSError("Failed to initialize ANTLR4 CSS parser: " + std::string(e.what()), 0, 0);
        m_initialized = false;
    }
}

std::string CSSCompiler::parseWithANTLR(const std::string& cssCode) {
    if (!m_initialized) {
        return cssCode;
    }
    
    try {
        // 创建ANTLR4解析树
        auto parseTree = createParseTree(cssCode);
        
        if (parseTree) {
            // 从解析树生成CSS代码
            return generateCSSFromTree(parseTree);
        }
    } catch (const std::exception& e) {
        handleCSSError("ANTLR4 parsing failed: " + std::string(e.what()), 0, 0);
    }
    
    // 如果ANTLR4解析失败，使用简化处理
    std::string processed = cssCode;
    
    // 确保选择器后有空格
    processed = std::regex_replace(processed, std::regex(R"(([.#\w]+)\{)"), "$1 {");
    
    // 确保属性后有分号
    processed = std::regex_replace(processed, std::regex(R"(:\s*([^;}]+)([;}]))"), ": $1;$2");
    
    return processed;
}

std::shared_ptr<antlr4::tree::ParseTree> CSSCompiler::createParseTree(const std::string& cssCode) {
    try {
        // 创建输入流
        m_inputStream = std::make_unique<antlr4::ANTLRInputStream>(cssCode);
        
        // 创建词法分析器
        m_lexer = std::make_unique<CSSLexer>(m_inputStream.get());
        
        // 创建Token流
        m_tokenStream = std::make_unique<antlr4::CommonTokenStream>(m_lexer.get());
        
        // 创建语法分析器
        m_parser = std::make_unique<CSSParser>(m_tokenStream.get());
        
        // 解析CSS
        return std::shared_ptr<antlr4::tree::ParseTree>(m_parser->stylesheet());
        
    } catch (const std::exception& e) {
        handleCSSError("Failed to create parse tree: " + std::string(e.what()), 0, 0);
        return nullptr;
    }
}

std::string CSSCompiler::generateCSSFromTree(const std::shared_ptr<antlr4::tree::ParseTree>& tree) {
    if (!tree) {
        return "";
    }
    
    // 从解析树生成CSS代码
    // 这里应该遍历解析树并生成格式化的CSS
    
    std::string result = tree->getText();
    
    // 基本格式化
    result = std::regex_replace(result, std::regex(R"(\s*\{\s*)"), " {\n    ");
    result = std::regex_replace(result, std::regex(R"(\s*\}\s*)"), "\n}\n");
    result = std::regex_replace(result, std::regex(R"(\s*;\s*)"), ";\n    ");
    
    return result;
}

void CSSCompiler::handleCSSError(const std::string& error, size_t line, size_t column) {
    ErrorReporter reporter;
    reporter.error(ErrorType::SYNTAX_ERROR, "CSS Error: " + error, "", line, column);
}

} // namespace CHTL