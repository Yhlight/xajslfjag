#pragma once

#include "../../Util/Common.h"
#include "../../../third-party/antlr/ANTLR4/include/antlr4-runtime.h"
#include "../../../third-party/antlr/generated/CSS/CSSLexer.h"
#include "../../../third-party/antlr/generated/CSS/CSSParser.h"
#include "../../../third-party/antlr/generated/CSS/CSSParserBaseListener.h"

namespace CHTL::CSS {

// CSS解析配置
struct CSSParserConfig {
    bool enableMinification = false;
    bool preserveComments = false;
    bool validateSyntax = true;
    bool enableSourceMap = false;
};

// CSS解析结果
struct CSSParseResult {
    String css;
    StringVector warnings;
    StringVector errors;
    bool success = false;
    double parseTime = 0.0;
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

// CSS AST节点访问器
class CSSASTVisitor : public CSSParserBaseListener {
public:
    explicit CSSASTVisitor(const CSSParserConfig& config);
    
    // 访问器方法
    void enterStylesheet(CSSParser::StylesheetContext* ctx) override;
    void exitStylesheet(CSSParser::StylesheetContext* ctx) override;
    
    void enterRuleset(CSSParser::RulesetContext* ctx) override;
    void exitRuleset(CSSParser::RulesetContext* ctx) override;
    
    void enterDeclaration(CSSParser::DeclarationContext* ctx) override;
    void exitDeclaration(CSSParser::DeclarationContext* ctx) override;
    
    void enterSelector(CSSParser::SelectorContext* ctx) override;
    void exitSelector(CSSParser::SelectorContext* ctx) override;
    
    // 获取生成的CSS
    String getGeneratedCSS() const { return generatedCSS; }
    StringVector getWarnings() const { return warnings; }
    StringVector getErrors() const { return errors; }
    
private:
    CSSParserConfig config;
    String generatedCSS;
    StringVector warnings;
    StringVector errors;
    int indentLevel = 0;
    
    // 工具方法
    String getIndent() const;
    void addWarning(const String& message);
    void addError(const String& message);
};

// CSS解析器
class Parser {
public:
    explicit Parser(const CSSParserConfig& config = CSSParserConfig());
    
    // 主要解析方法
    CSSParseResult parse(const String& cssCode);
    CSSParseResult parseFromFile(const String& filePath);
    
    // 配置管理
    void setConfig(const CSSParserConfig& config);
    CSSParserConfig getConfig() const;
    
    // 静态工具方法
    static bool isValidCSS(const String& cssCode);
    static String minifyCSS(const String& cssCode);
    static String formatCSS(const String& cssCode);
    
private:
    CSSParserConfig config;
    
    // 内部解析方法
    CSSParseResult parseInternal(antlr4::ANTLRInputStream& input);
    void handleParseErrors(const std::vector<antlr4::RecognitionException>& errors, 
                          CSSParseResult& result);
};

} // namespace CHTL::CSS