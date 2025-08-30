#include "JSCompiler.h"
#include <regex>
#include <sstream>

// ANTLR4运行时
#include "antlr4-runtime.h"
#include "ANTLRInputStream.h"
#include "CommonTokenStream.h"
#include "tree/ParseTree.h"

// 生成的JavaScript解析器
#include "SimpleJSLexer.h"
#include "SimpleJSParser.h"
#include "SimpleJSParserBaseListener.h"

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
    try {
        // 初始化ANTLR4 JavaScript解析器
        m_initialized = true;
    } catch (const std::exception& e) {
        handleJSError("Failed to initialize ANTLR4 JavaScript parser: " + std::string(e.what()), 0, 0);
        m_initialized = false;
    }
}

std::string JSCompiler::parseWithANTLR(const std::string& jsCode) {
    if (!m_initialized) {
        return jsCode;
    }
    
    try {
        // 创建ANTLR4解析树
        auto parseTree = createParseTree(jsCode);
        
        if (parseTree) {
            // 从解析树生成JavaScript代码
            return generateJSFromTree(parseTree);
        }
    } catch (const std::exception& e) {
        handleJSError("ANTLR4 parsing failed: " + std::string(e.what()), 0, 0);
    }
    
    // 如果ANTLR4解析失败，使用简化处理
    std::string processed = jsCode;
    
    // 确保语句以分号结尾
    processed = std::regex_replace(processed, std::regex(R"(([^;{}\s])\s*\n)"), "$1;\n");
    
    // 确保函数声明格式正确
    processed = std::regex_replace(processed, std::regex(R"(function\s*([^(]*)\s*\()"), "function $1(");
    
    return processed;
}

std::shared_ptr<antlr4::tree::ParseTree> JSCompiler::createParseTree(const std::string& jsCode) {
    try {
        // 创建输入流
        m_inputStream = std::make_unique<antlr4::ANTLRInputStream>(jsCode);
        
        // 创建词法分析器
        m_lexer = std::make_unique<SimpleJSLexer>(m_inputStream.get());
        
        // 创建Token流
        m_tokenStream = std::make_unique<antlr4::CommonTokenStream>(m_lexer.get());
        
        // 创建语法分析器
        m_parser = std::make_unique<SimpleJSParser>(m_tokenStream.get());
        
        // 解析JavaScript
        return std::shared_ptr<antlr4::tree::ParseTree>(m_parser->program());
        
    } catch (const std::exception& e) {
        handleJSError("Failed to create parse tree: " + std::string(e.what()), 0, 0);
        return nullptr;
    }
}

std::string JSCompiler::generateJSFromTree(const std::shared_ptr<antlr4::tree::ParseTree>& tree) {
    if (!tree) {
        return "";
    }
    
    // 从解析树生成JavaScript代码
    std::string result = tree->getText();
    
    // 基本格式化
    result = std::regex_replace(result, std::regex(R"(\s*\{\s*)"), " {\n    ");
    result = std::regex_replace(result, std::regex(R"(\s*\}\s*)"), "\n}\n");
    result = std::regex_replace(result, std::regex(R"(\s*;\s*)"), ";\n");
    
    return result;
}

void JSCompiler::handleJSError(const std::string& error, size_t line, size_t column) {
    ErrorReporter reporter;
    reporter.error(ErrorType::SYNTAX_ERROR, "JavaScript Error: " + error, "", line, column);
}

} // namespace CHTL