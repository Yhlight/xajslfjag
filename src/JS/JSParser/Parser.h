#pragma once

#include "../../Util/Common.h"
#include "../../../third-party/antlr/ANTLR4/include/antlr4-runtime.h"
#include "../../../third-party/antlr/generated/JS/SimpleJSLexer.h"
#include "../../../third-party/antlr/generated/JS/SimpleJSParser.h"
#include "../../../third-party/antlr/generated/JS/SimpleJSParserBaseListener.h"

namespace CHTL::JS {

// JavaScript解析配置
struct JSParserConfig {
    bool enableMinification = false;
    bool preserveComments = false;
    bool validateSyntax = true;
    bool enableSourceMap = false;
    bool strictMode = false;
    bool esModuleMode = true;
};

// JavaScript解析结果
struct JSParseResult {
    String javascript;
    StringVector warnings;
    StringVector errors;
    bool success = false;
    double parseTime = 0.0;
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

// JavaScript AST节点访问器
class JSASTVisitor : public SimpleJSParserBaseListener {
public:
    explicit JSASTVisitor(const JSParserConfig& config);
    
    // 访问器方法
    void enterProgram(SimpleJSParser::ProgramContext* ctx) override;
    void exitProgram(SimpleJSParser::ProgramContext* ctx) override;
    
    void enterFunctionDeclaration(SimpleJSParser::FunctionDeclarationContext* ctx) override;
    void exitFunctionDeclaration(SimpleJSParser::FunctionDeclarationContext* ctx) override;
    
    void enterVarDeclaration(SimpleJSParser::VarDeclarationContext* ctx) override;
    void exitVarDeclaration(SimpleJSParser::VarDeclarationContext* ctx) override;
    
    void enterExpressionStatement(SimpleJSParser::ExpressionStatementContext* ctx) override;
    void exitExpressionStatement(SimpleJSParser::ExpressionStatementContext* ctx) override;
    
    void enterBlock(SimpleJSParser::BlockContext* ctx) override;
    void exitBlock(SimpleJSParser::BlockContext* ctx) override;
    
    // 获取生成的JavaScript
    String getGeneratedJS() const { return generatedJS; }
    StringVector getWarnings() const { return warnings; }
    StringVector getErrors() const { return errors; }
    
private:
    JSParserConfig config;
    String generatedJS;
    StringVector warnings;
    StringVector errors;
    int indentLevel = 0;
    
    // 工具方法
    String getIndent() const;
    void addWarning(const String& message);
    void addError(const String& message);
};

// JavaScript解析器
class Parser {
public:
    explicit Parser(const JSParserConfig& config = JSParserConfig());
    
    // 主要解析方法
    JSParseResult parse(const String& jsCode);
    JSParseResult parseFromFile(const String& filePath);
    
    // 配置管理
    void setConfig(const JSParserConfig& config);
    JSParserConfig getConfig() const;
    
    // 静态工具方法
    static bool isValidJS(const String& jsCode);
    static String minifyJS(const String& jsCode);
    static String formatJS(const String& jsCode);
    
private:
    JSParserConfig config;
    
    // 内部解析方法
    JSParseResult parseInternal(antlr4::ANTLRInputStream& input);
    void handleParseErrors(const std::vector<antlr4::RecognitionException>& errors, 
                          JSParseResult& result);
};

} // namespace CHTL::JS