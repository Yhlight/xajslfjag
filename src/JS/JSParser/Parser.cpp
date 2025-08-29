#include "Parser.h"
#include "../../Util/StringUtils.h"
#include <chrono>
#include <fstream>
#include <sstream>

using namespace antlr4;

namespace CHTL::JS {

// JSASTVisitor 实现
JSASTVisitor::JSASTVisitor(const JSParserConfig& config) : config(config) {}

void JSASTVisitor::enterProgram(SimpleJSParser::ProgramContext* ctx) {
    // 开始程序解析
    if (config.strictMode) {
        generatedJS += "\"use strict\";\n";
    }
}

void JSASTVisitor::exitProgram(SimpleJSParser::ProgramContext* ctx) {
    // 完成程序解析
}

void JSASTVisitor::enterFunctionDeclaration(SimpleJSParser::FunctionDeclarationContext* ctx) {
    // 处理函数声明
    String functionText = ctx->getText();
    
    if (config.enableMinification) {
        generatedJS += functionText;
    } else {
        generatedJS += getIndent() + functionText + "\n";
    }
}

void JSASTVisitor::exitFunctionDeclaration(SimpleJSParser::FunctionDeclarationContext* ctx) {
    // 函数声明处理完成
}

void JSASTVisitor::enterVarDeclaration(SimpleJSParser::VarDeclarationContext* ctx) {
    // 处理变量声明
    String varText = ctx->getText();
    
    if (config.enableMinification) {
        generatedJS += varText;
    } else {
        generatedJS += getIndent() + varText + "\n";
    }
}

void JSASTVisitor::exitVarDeclaration(SimpleJSParser::VarDeclarationContext* ctx) {
    // 变量声明处理完成
}

void JSASTVisitor::enterExpressionStatement(SimpleJSParser::ExpressionStatementContext* ctx) {
    // 处理表达式语句
    String exprText = ctx->getText();
    
    if (config.enableMinification) {
        generatedJS += exprText;
    } else {
        generatedJS += getIndent() + exprText + "\n";
    }
}

void JSASTVisitor::exitExpressionStatement(SimpleJSParser::ExpressionStatementContext* ctx) {
    // 表达式语句处理完成
}

void JSASTVisitor::enterBlock(SimpleJSParser::BlockContext* ctx) {
    // 处理代码块
    if (config.enableMinification) {
        generatedJS += "{";
    } else {
        generatedJS += getIndent() + "{\n";
        indentLevel++;
    }
}

void JSASTVisitor::exitBlock(SimpleJSParser::BlockContext* ctx) {
    // 代码块处理完成
    if (config.enableMinification) {
        generatedJS += "}";
    } else {
        indentLevel--;
        generatedJS += getIndent() + "}\n";
    }
}

String JSASTVisitor::getIndent() const {
    return String(indentLevel * 2, ' ');
}

void JSASTVisitor::addWarning(const String& message) {
    warnings.push_back(message);
}

void JSASTVisitor::addError(const String& message) {
    errors.push_back(message);
}

// Parser 实现
Parser::Parser(const JSParserConfig& config) : config(config) {}

JSParseResult Parser::parse(const String& jsCode) {
    ANTLRInputStream input(jsCode);
    return parseInternal(input);
}

JSParseResult Parser::parseFromFile(const String& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        JSParseResult result;
        result.errors.push_back("无法打开文件: " + filePath);
        return result;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parse(buffer.str());
}

JSParseResult Parser::parseInternal(ANTLRInputStream& input) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    JSParseResult result;
    
    try {
        // 创建词法分析器
        SimpleJSLexer lexer(&input);
        CommonTokenStream tokens(&lexer);
        
        // 创建语法分析器
        SimpleJSParser parser(&tokens);
        
        // 解析程序
        auto tree = parser.program();
        
        // 创建访问器并遍历AST
        JSASTVisitor visitor(config);
        tree::ParseTreeWalker::DEFAULT.walk(&visitor, tree);
        
        // 获取结果
        result.javascript = visitor.getGeneratedJS();
        result.warnings = visitor.getWarnings();
        result.errors = visitor.getErrors();
        result.success = result.errors.empty();
        
        // 后处理
        if (config.enableMinification && result.success) {
            result.javascript = minifyJS(result.javascript);
        }
        
    } catch (const std::exception& e) {
        result.errors.push_back("JavaScript解析异常: " + String(e.what()));
        result.success = false;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    result.parseTime = duration.count() / 1000.0; // 转换为毫秒
    
    return result;
}

void Parser::handleParseErrors(const std::vector<antlr4::RecognitionException>& errors,
                              JSParseResult& result) {
    for (const auto& error : errors) {
        result.errors.push_back("解析错误: " + String(error.what()));
    }
}

void Parser::setConfig(const JSParserConfig& config) {
    this->config = config;
}

JSParserConfig Parser::getConfig() const {
    return config;
}

bool Parser::isValidJS(const String& jsCode) {
    Parser parser;
    auto result = parser.parse(jsCode);
    return result.success;
}

String Parser::minifyJS(const String& jsCode) {
    String minified = jsCode;
    
    // 移除多余空白
    minified = Util::StringUtils::replace_all(minified, "\n", "");
    minified = Util::StringUtils::replace_all(minified, "\r", "");
    minified = Util::StringUtils::replace_all(minified, "\t", "");
    
    // 移除多余空格
    minified = Util::StringUtils::replace_all(minified, "  ", " ");
    minified = Util::StringUtils::replace_all(minified, " {", "{");
    minified = Util::StringUtils::replace_all(minified, "{ ", "{");
    minified = Util::StringUtils::replace_all(minified, " }", "}");
    minified = Util::StringUtils::replace_all(minified, "} ", "}");
    minified = Util::StringUtils::replace_all(minified, " ;", ";");
    minified = Util::StringUtils::replace_all(minified, "; ", ";");
    minified = Util::StringUtils::replace_all(minified, " =", "=");
    minified = Util::StringUtils::replace_all(minified, "= ", "=");
    
    return Util::StringUtils::trim(minified);
}

String Parser::formatJS(const String& jsCode) {
    JSParserConfig config;
    config.enableMinification = false;
    
    Parser parser(config);
    auto result = parser.parse(jsCode);
    
    return result.success ? result.javascript : jsCode;
}

} // namespace CHTL::JS