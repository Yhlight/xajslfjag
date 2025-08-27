#pragma once

#include <memory>
#include <vector>
#include <stack>
#include "CHTLJSLexer/Lexer.h"
#include "CHTLJSLexer/GlobalMap.h"
#include "CHTLJSNode.h"
#include "CHTLJSState.h"
#include "CHTLJSContext.h"

namespace CHTLJS {

// CHTL JS解析器
class CHTLJSParser {
public:
    CHTLJSParser(std::shared_ptr<Lexer> lexer, std::shared_ptr<GlobalMap> globalMap);
    ~CHTLJSParser();
    
    // 解析入口
    NodePtr Parse();
    
    // 错误处理
    void ReportError(const std::string& message);
    std::vector<std::string> GetErrors() const { return errors; }
    bool HasErrors() const { return !errors.empty(); }
    
    // 调试模式
    void SetDebugMode(bool debug) { debugMode = debug; }
    
private:
    std::shared_ptr<Lexer> lexer;
    std::shared_ptr<GlobalMap> globalMap;
    std::shared_ptr<CHTLJSContext> context;
    std::shared_ptr<CHTLJSState> state;
    
    Token currentToken;
    std::vector<std::string> errors;
    bool debugMode;
    
    // Token操作
    void Consume();
    Token Peek(int offset = 0);
    bool Match(TokenType type);
    bool MatchAny(std::initializer_list<TokenType> types);
    void Expect(TokenType type, const std::string& message);
    
    // 解析方法 - 顶层
    NodePtr ParseProgram();
    NodePtr ParseStatement();
    NodePtr ParseExpression();
    
    // 解析方法 - CHTL JS特有
    NodePtr ParseEnhancedSelector();
    NodePtr ParseArrowAccess(NodePtr object);
    NodePtr ParseVirDeclaration();
    NodePtr ParseCHTLJSFunction(const std::string& name);
    NodePtr ParseEventBinding(NodePtr target);
    
    // 解析方法 - 表达式
    NodePtr ParsePrimaryExpression();
    NodePtr ParsePostfixExpression();
    NodePtr ParseUnaryExpression();
    NodePtr ParseBinaryExpression(int precedence = 0);
    NodePtr ParseAssignmentExpression();
    NodePtr ParseConditionalExpression();
    
    // 解析方法 - 语句
    NodePtr ParseBlockStatement();
    NodePtr ParseVariableDeclaration();
    NodePtr ParseFunctionDeclaration();
    NodePtr ParseIfStatement();
    NodePtr ParseForStatement();
    NodePtr ParseWhileStatement();
    NodePtr ParseReturnStatement();
    NodePtr ParseExpressionStatement();
    
    // 解析方法 - 字面量
    NodePtr ParseLiteral();
    NodePtr ParseObjectLiteral();
    NodePtr ParseArrayLiteral();
    NodePtr ParseFunctionExpression();
    NodePtr ParseArrowFunction();
    
    // 解析方法 - CHTL JS函数内容
    NodePtr ParseListenContent();
    NodePtr ParseDelegateContent();
    NodePtr ParseAnimateContent();
    
    // 辅助方法
    bool IsAssignmentOperator(TokenType type);
    bool IsBinaryOperator(TokenType type);
    int GetOperatorPrecedence(TokenType type);
    bool IsStatementStart();
    bool IsCHTLJSFunctionStart();
    
    // 虚对象处理
    void ProcessVirtualObject(const std::string& name, NodePtr value);
    NodePtr ResolveVirtualObjectProperty(const std::string& name, const std::string& property);
};

} // namespace CHTLJS