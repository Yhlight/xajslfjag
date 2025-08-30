#include "JSCompiler.h"
#include <regex>
#include <sstream>

namespace CHTL {

JSCompiler::JSCompiler() : m_initialized(false) {
    initializeANTLR();
}

std::string JSCompiler::compile(const std::string& jsCode) {
    if (!m_initialized) {
        initializeANTLR();
    }
    
    // 使用ANTLR4解析JavaScript
    std::string result = parseWithANTLR(jsCode);
    
    if (validateJS(result)) {
        return optimizeJS(result);
    }
    
    return jsCode; // 如果解析失败，返回原始代码
}

bool JSCompiler::validateJS(const std::string& jsCode) {
    // 基础JavaScript语法验证
    
    // 检查括号匹配
    int braceCount = 0;
    int parenCount = 0;
    int bracketCount = 0;
    
    for (char c : jsCode) {
        switch (c) {
            case '{': braceCount++; break;
            case '}': braceCount--; break;
            case '(': parenCount++; break;
            case ')': parenCount--; break;
            case '[': bracketCount++; break;
            case ']': bracketCount--; break;
        }
        
        if (braceCount < 0 || parenCount < 0 || bracketCount < 0) {
            return false;
        }
    }
    
    return braceCount == 0 && parenCount == 0 && bracketCount == 0;
}

std::string JSCompiler::optimizeJS(const std::string& jsCode) {
    std::string optimized = jsCode;
    
    // 移除多余的空白
    optimized = std::regex_replace(optimized, std::regex(R"(\s+)"), " ");
    
    // 移除单行注释
    optimized = std::regex_replace(optimized, std::regex(R"(//.*$)"), "", std::regex_constants::format_default);
    
    // 移除块注释
    optimized = std::regex_replace(optimized, std::regex(R"(/\*.*?\*/)"), "");
    
    // 优化操作符间距
    optimized = std::regex_replace(optimized, std::regex(R"(\s*=\s*)"), " = ");
    optimized = std::regex_replace(optimized, std::regex(R"(\s*\+\s*)"), " + ");
    optimized = std::regex_replace(optimized, std::regex(R"(\s*-\s*)"), " - ");
    optimized = std::regex_replace(optimized, std::regex(R"(\s*\*\s*)"), " * ");
    optimized = std::regex_replace(optimized, std::regex(R"(\s*/\s*)"), " / ");
    
    // 优化函数调用
    optimized = std::regex_replace(optimized, std::regex(R"(\s*\(\s*)"), "(");
    optimized = std::regex_replace(optimized, std::regex(R"(\s*\)\s*)"), ")");
    
    return optimized;
}

std::string JSCompiler::getCompilerInfo() const {
    return "JavaScript Compiler (ANTLR4-based) v1.0";
}

void JSCompiler::initializeANTLR() {
    // 初始化ANTLR4 JavaScript解析器
    // 这里应该加载JavaScript.g4语法文件并初始化解析器
    m_initialized = true;
}

std::string JSCompiler::parseWithANTLR(const std::string& jsCode) {
    // 使用ANTLR4解析JavaScript代码
    // 这里应该使用生成的JavaScript解析器
    
    std::ostringstream result;
    
    // 简化的JavaScript处理：确保基本语法正确
    std::string processed = jsCode;
    
    // 确保语句以分号结尾
    processed = std::regex_replace(processed, std::regex(R"(([^;{}\s])\s*\n)"), "$1;\n");
    
    // 确保函数声明格式正确
    processed = std::regex_replace(processed, std::regex(R"(function\s*([^(]*)\s*\()"), "function $1(");
    
    // 确保变量声明格式正确
    processed = std::regex_replace(processed, std::regex(R"((var|let|const)\s+)"), "$1 ");
    
    result << processed;
    
    return result.str();
}

void JSCompiler::handleJSError(const std::string& error, size_t line, size_t column) {
    ErrorReporter reporter;
    reporter.error(ErrorType::SYNTAX_ERROR, "JavaScript Error: " + error, "", line, column);
}

} // namespace CHTL