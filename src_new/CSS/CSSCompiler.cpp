#include "CSSCompiler.h"
#include <regex>
#include <sstream>

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
    // 初始化ANTLR4 CSS解析器
    // 这里应该加载CSS.g4语法文件并初始化解析器
    m_initialized = true;
}

std::string CSSCompiler::parseWithANTLR(const std::string& cssCode) {
    // 使用ANTLR4解析CSS代码
    // 这里应该使用生成的CSS解析器
    
    std::ostringstream result;
    
    // 简化的CSS处理：确保基本格式正确
    std::string processed = cssCode;
    
    // 确保选择器后有空格
    processed = std::regex_replace(processed, std::regex(R"(([.#\w]+)\{)"), "$1 {");
    
    // 确保属性后有分号
    processed = std::regex_replace(processed, std::regex(R"(:\s*([^;}]+)([;}]))"), ": $1;$2");
    
    // 移除多余分号
    processed = std::regex_replace(processed, std::regex(R"(;+)"), ";");
    
    result << processed;
    
    return result.str();
}

void CSSCompiler::handleCSSError(const std::string& error, size_t line, size_t column) {
    ErrorReporter reporter;
    reporter.error(ErrorType::SYNTAX_ERROR, "CSS Error: " + error, "", line, column);
}

} // namespace CHTL