#ifndef CHTLJS_PARSER_H
#define CHTLJS_PARSER_H

#include <memory>
#include <vector>
#include <string>
#include "../CHTLJSLexer/Lexer.h"
#include "../CHTLJSLexer/Token.h"
#include "../CHTLJSContext/Context.h"
#include "../CHTLJSNode/BaseNode.h"
#include "../CHTLJSNode/ProgramNode.h"

namespace CHTLJS {

// 解析器配置
struct ParserConfig {
    bool strictMode = false;                // 严格模式
    bool allowUnquotedLiterals = true;     // 允许无修饰字面量
};

// CHTL JS解析器
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
    
    // 语句解析
    std::shared_ptr<ASTNode> parseStatement();
    std::shared_ptr<ASTNode> parseExpressionStatement();
    std::shared_ptr<ASTNode> parseVariableDeclaration();
    std::shared_ptr<ASTNode> parseFunctionDeclaration();
    std::shared_ptr<ASTNode> parseIfStatement();
    std::shared_ptr<ASTNode> parseForStatement();
    std::shared_ptr<ASTNode> parseWhileStatement();
    std::shared_ptr<ASTNode> parseReturnStatement();
    
    // CHTL JS特有语法解析
    std::shared_ptr<ASTNode> parseModuleBlock();
    std::shared_ptr<ASTNode> parseEnhancedSelector();
    std::shared_ptr<ASTNode> parseListenBlock();
    std::shared_ptr<ASTNode> parseDelegateBlock();
    std::shared_ptr<ASTNode> parseAnimateBlock();
    std::shared_ptr<ASTNode> parseVirtualObject();
    std::shared_ptr<ASTNode> parseINeverAway();
    std::shared_ptr<ASTNode> parseEventBinding();
    
    // 表达式解析（优先级递减）
    std::shared_ptr<ASTNode> parseExpression();
    std::shared_ptr<ASTNode> parseAssignment();
    std::shared_ptr<ASTNode> parseOr();
    std::shared_ptr<ASTNode> parseAnd();
    std::shared_ptr<ASTNode> parseEquality();
    std::shared_ptr<ASTNode> parseComparison();
    std::shared_ptr<ASTNode> parseAddition();
    std::shared_ptr<ASTNode> parseMultiplication();
    std::shared_ptr<ASTNode> parseUnary();
    std::shared_ptr<ASTNode> parsePostfix();
    std::shared_ptr<ASTNode> parseCall();
    std::shared_ptr<ASTNode> parseMember();
    std::shared_ptr<ASTNode> parsePrimary();
    
    // 辅助解析方法
    std::shared_ptr<ASTNode> parseObjectLiteral();
    std::shared_ptr<ASTNode> parseArrayLiteral();
    std::shared_ptr<ASTNode> parseFunctionExpression();
    std::shared_ptr<ASTNode> parseArrowFunction();
    std::vector<std::string> parseParameterList();
    std::shared_ptr<ASTNode> parseBlock();
    
    // CHTL JS函数解析
    std::shared_ptr<ASTNode> parseCHTLJSFunction(const std::string& functionName);
    std::unordered_map<std::string, std::shared_ptr<ASTNode>> parseKeyValuePairs();
    
    // 动画相关解析
    std::shared_ptr<ASTNode> parseAnimateBegin();
    std::shared_ptr<ASTNode> parseAnimateWhen();
    std::shared_ptr<ASTNode> parseAnimateEnd();
    
    // 辅助方法
    std::string parseIdentifier();
    std::string parseString();
    EnhancedSelectorNode::SelectorType determineSelectorType(const std::string& selector);
    
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

} // namespace CHTLJS

#endif // CHTLJS_PARSER_H