#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include <memory>
#include <vector>
#include <string>
#include "../CHTLLexer/Lexer.h"
#include "../CHTLLexer/Token.h"
#include "../CHTLContext/Context.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ProgramNode.h"

namespace CHTL {

// 解析器配置
struct ParserConfig {
    bool strictMode = false;                // 严格模式
    bool allowUnquotedLiterals = true;     // 允许无修饰字面量
    bool enableCEEquivalence = true;       // 启用CE对等式
};

// CHTL解析器
class Parser {
public:
    Parser(std::shared_ptr<Lexer> lexer, std::shared_ptr<CompileContext> context,
           const ParserConfig& config = ParserConfig());
    ~Parser() = default;
    
    // 解析整个程序
    std::shared_ptr<ProgramNode> parse();
    
    // 获取解析错误
    const std::vector<std::string>& getErrors() const { return errors_; }
    bool hasErrors() const { return !errors_.empty(); }

private:
    std::shared_ptr<Lexer> lexer_;
    std::shared_ptr<CompileContext> context_;
    ParserConfig config_;
    std::vector<std::string> errors_;
    
    // 当前Token
    std::shared_ptr<Token> current_;
    std::shared_ptr<Token> previous_;
    
    // Token操作
    void advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    std::shared_ptr<Token> consume(TokenType type, const std::string& message);
    bool isAtEnd() const;
    
    // 错误处理
    void error(const std::string& message);
    void error(const Token& token, const std::string& message);
    void synchronize();
    
    // 顶层解析
    std::shared_ptr<ASTNode> parseTopLevel();
    std::shared_ptr<ASTNode> parseUseStatement();
    std::shared_ptr<ASTNode> parseNamespace();
    std::shared_ptr<ASTNode> parseConfiguration();
    std::shared_ptr<ASTNode> parseTemplate();
    std::shared_ptr<ASTNode> parseCustom();
    std::shared_ptr<ASTNode> parseOrigin();
    std::shared_ptr<ASTNode> parseImport();
    std::shared_ptr<ASTNode> parseInfo();
    std::shared_ptr<ASTNode> parseExport();
    
    // 元素解析
    std::shared_ptr<ElementNode> parseElement();
    std::shared_ptr<ASTNode> parseElementContent();
    std::shared_ptr<TextNode> parseTextBlock();
    std::shared_ptr<ASTNode> parseStyleBlock();
    std::shared_ptr<ASTNode> parseScriptBlock();
    
    // 属性解析
    std::vector<std::shared_ptr<AttributeNode>> parseAttributes();
    std::shared_ptr<AttributeNode> parseAttribute();
    std::string parseAttributeValue();
    
    // 样式解析
    std::shared_ptr<ASTNode> parseStyleContent();
    std::shared_ptr<ASTNode> parseStyleRule();
    std::shared_ptr<ASTNode> parseSelector();
    std::shared_ptr<ASTNode> parseStyleProperty();
    
    // 模板和自定义解析
    std::shared_ptr<ASTNode> parseTemplateContent(TemplateType type);
    std::shared_ptr<ASTNode> parseCustomContent(CustomType type);
    std::shared_ptr<ASTNode> parseTemplateUse();
    std::shared_ptr<ASTNode> parseCustomUse();
    
    // 操作符解析
    std::shared_ptr<ASTNode> parseDeleteOperation();
    std::shared_ptr<ASTNode> parseInsertOperation();
    std::shared_ptr<ASTNode> parseInheritOperation();
    std::shared_ptr<ASTNode> parseExceptConstraint();
    
    // 导入解析
    ImportType determineImportType();
    std::string parseImportPath();
    std::optional<std::string> parseAlias();
    
    // 配置解析
    std::shared_ptr<ConfigNode> parseConfigContent();
    std::pair<std::string, std::string> parseConfigProperty();
    
    // 辅助方法
    std::string parseIdentifier();
    std::string parseString();
    std::string parseUnquotedLiteral();
    bool isHtmlTag(const std::string& name);
    bool isTemplateType(TokenType type);
    bool isCustomType(TokenType type);
    
    // 状态管理
    void enterState(StateType state);
    void exitState();
    bool isInState(StateType state) const;
};

// 解析异常
class ParseException : public std::exception {
public:
    explicit ParseException(const std::string& message) : msg_(message) {}
    const char* what() const noexcept override { return msg_.c_str(); }
    
private:
    std::string msg_;
};

} // namespace CHTL

#endif // CHTL_PARSER_H