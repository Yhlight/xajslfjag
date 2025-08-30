#include "Parser.h"
#include "../../Util/StringUtils.h"
#include <chrono>
#include <fstream>
#include <sstream>

using namespace antlr4;

namespace CHTL::CSS {

// CSSASTVisitor 实现
CSSASTVisitor::CSSASTVisitor(const CSSParserConfig& config) : config(config) {}

void CSSASTVisitor::enterStylesheet(CSSParser::StylesheetContext* ctx) {
    // 开始样式表解析
}

void CSSASTVisitor::exitStylesheet(CSSParser::StylesheetContext* ctx) {
    // 完成样式表解析
}

void CSSASTVisitor::enterRuleset(CSSParser::RulesetContext* ctx) {
    // 开始规则集解析
    if (config.enableMinification) {
        // 压缩模式，不添加换行和缩进
    } else {
        generatedCSS += getIndent();
    }
}

void CSSASTVisitor::exitRuleset(CSSParser::RulesetContext* ctx) {
    // 完成规则集解析
    if (!config.enableMinification) {
        generatedCSS += "\n";
    }
}

void CSSASTVisitor::enterDeclaration(CSSParser::DeclarationContext* ctx) {
    // 处理CSS声明
    if (ctx->property() && ctx->expr()) {
        String property = ctx->property()->getText();
        String value = ctx->expr()->getText();
        
        if (config.enableMinification) {
            generatedCSS += property + ":" + value + ";";
        } else {
            generatedCSS += getIndent() + property + ": " + value + ";\n";
        }
    }
}

void CSSASTVisitor::exitDeclaration(CSSParser::DeclarationContext* ctx) {
    // 声明处理完成
}

void CSSASTVisitor::enterSelector(CSSParser::SelectorContext* ctx) {
    // 处理选择器
    String selector = ctx->getText();
    
    if (config.enableMinification) {
        generatedCSS += selector + "{";
    } else {
        generatedCSS += selector + " {\n";
        indentLevel++;
    }
}

void CSSASTVisitor::exitSelector(CSSParser::SelectorContext* ctx) {
    // 选择器处理完成
    if (config.enableMinification) {
        generatedCSS += "}";
    } else {
        indentLevel--;
        generatedCSS += getIndent() + "}\n";
    }
}

String CSSASTVisitor::getIndent() const {
    return String(indentLevel * 2, ' ');
}

void CSSASTVisitor::addWarning(const String& message) {
    warnings.push_back(message);
}

void CSSASTVisitor::addError(const String& message) {
    errors.push_back(message);
}

// Parser 实现
Parser::Parser(const CSSParserConfig& config) : config(config) {}

CSSParseResult Parser::parse(const String& cssCode) {
    ANTLRInputStream input(cssCode);
    return parseInternal(input);
}

CSSParseResult Parser::parseFromFile(const String& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        CSSParseResult result;
        result.errors.push_back("无法打开文件: " + filePath);
        return result;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parse(buffer.str());
}

CSSParseResult Parser::parseInternal(ANTLRInputStream& input) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CSSParseResult result;
    
    try {
        // 创建词法分析器
        CSSLexer lexer(&input);
        CommonTokenStream tokens(&lexer);
        
        // 创建语法分析器
        CSSParser parser(&tokens);
        
        // 解析样式表
        auto tree = parser.stylesheet();
        
        // 创建访问器并遍历AST
        CSSASTVisitor visitor(config);
        tree::ParseTreeWalker::DEFAULT.walk(&visitor, tree);
        
        // 获取结果
        result.css = visitor.getGeneratedCSS();
        result.warnings = visitor.getWarnings();
        result.errors = visitor.getErrors();
        result.success = result.errors.empty();
        
        // 后处理
        if (config.enableMinification && result.success) {
            result.css = minifyCSS(result.css);
        }
        
    } catch (const std::exception& e) {
        result.errors.push_back("CSS解析异常: " + String(e.what()));
        result.success = false;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    result.parseTime = duration.count() / 1000.0; // 转换为毫秒
    
    return result;
}

void Parser::handleParseErrors(const std::vector<antlr4::RecognitionException>& errors,
                              CSSParseResult& result) {
    for (const auto& error : errors) {
        result.errors.push_back("解析错误: " + String(error.what()));
    }
}

void Parser::setConfig(const CSSParserConfig& config) {
    this->config = config;
}

CSSParserConfig Parser::getConfig() const {
    return config;
}

bool Parser::isValidCSS(const String& cssCode) {
    Parser parser;
    auto result = parser.parse(cssCode);
    return result.success;
}

String Parser::minifyCSS(const String& cssCode) {
    String minified = cssCode;
    
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
    minified = Util::StringUtils::replace_all(minified, " :", ":");
    minified = Util::StringUtils::replace_all(minified, ": ", ":");
    
    return Util::StringUtils::trim(minified);
}

String Parser::formatCSS(const String& cssCode) {
    CSSParserConfig config;
    config.enableMinification = false;
    
    Parser parser(config);
    auto result = parser.parse(cssCode);
    
    return result.success ? result.css : cssCode;
}

} // namespace CHTL::CSS