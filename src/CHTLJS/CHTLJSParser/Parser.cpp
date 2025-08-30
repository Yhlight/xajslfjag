#include "Parser.h"
#include "../CHTLJSNode/ModuleNode.h"
#include "../CHTLJSNode/SelectorNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/AnimateNode.h"
#include "../CHTLJSNode/VirtualObjectNode.h"
#include "../CHTLJSNode/OperatorNode.h"
#include "../CHTLJSNode/JavaScriptNode.h"
#include <sstream>

namespace CHTLJS {

Parser::Parser(std::shared_ptr<Lexer> lexer, std::shared_ptr<CompileContext> context,
               const ParserConfig& config)
    : lexer_(lexer), context_(context), config_(config) {
    // 读取第一个Token
    advance();
}

std::shared_ptr<ProgramNode> Parser::parse() {
    TokenLocation loc(1, 1, 0, 0);
    auto program = std::make_shared<ProgramNode>(context_->getSourceFile(), loc);
    
    // 解析语句列表
    while (!isAtEnd()) {
        try {
            auto statement = parseStatement();
            if (statement) {
                program->addStatement(statement);
            }
        } catch (const ParseException& e) {
            error(e.what());
            synchronize();
        }
    }
    
    return program;
}

void Parser::advance() {
    previous_ = current_;
    current_ = lexer_->nextToken();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return current_->getType() == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

std::shared_ptr<Token> Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return previous_;
    }
    
    error(*current_, message);
    throw ParseException(message);
}

bool Parser::isAtEnd() const {
    return current_->getType() == TokenType::EOF_TOKEN;
}

void Parser::error(const std::string& message) {
    errors_.push_back(message);
    context_->addError(message);
}

void Parser::error(const Token& token, const std::string& message) {
    std::stringstream ss;
    ss << "at " << token.getLocation().line << ":" << token.getLocation().column 
       << " '" << token.getLexeme() << "': " << message;
    error(ss.str());
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous_->getType() == TokenType::SEMICOLON) return;
        
        switch (current_->getType()) {
            case TokenType::KEYWORD_CONST:
            case TokenType::KEYWORD_LET:
            case TokenType::KEYWORD_VAR:
            case TokenType::KEYWORD_FUNCTION:
            case TokenType::KEYWORD_IF:
            case TokenType::KEYWORD_FOR:
            case TokenType::KEYWORD_WHILE:
            case TokenType::KEYWORD_RETURN:
            case TokenType::KEYWORD_MODULE:
            case TokenType::KEYWORD_VIR:
                return;
            default:
                break;
        }
        
        advance();
    }
}

std::shared_ptr<ASTNode> Parser::parseStatement() {
    // CHTL JS特有语句
    if (check(TokenType::KEYWORD_MODULE)) {
        return parseModuleBlock();
    }
    
    if (check(TokenType::KEYWORD_VIR)) {
        return parseVirtualObject();
    }
    
    // JavaScript语句
    if (match({TokenType::KEYWORD_CONST, TokenType::KEYWORD_LET, TokenType::KEYWORD_VAR})) {
        return parseVariableDeclaration();
    }
    
    if (check(TokenType::KEYWORD_FUNCTION)) {
        return parseFunctionDeclaration();
    }
    
    if (check(TokenType::KEYWORD_IF)) {
        return parseIfStatement();
    }
    
    if (check(TokenType::KEYWORD_FOR)) {
        return parseForStatement();
    }
    
    if (check(TokenType::KEYWORD_WHILE)) {
        return parseWhileStatement();
    }
    
    if (check(TokenType::KEYWORD_RETURN)) {
        return parseReturnStatement();
    }
    
    // 表达式语句
    return parseExpressionStatement();
}

std::shared_ptr<ASTNode> Parser::parseExpressionStatement() {
    auto expr = parseExpression();
    
    // 分号是可选的（在某些情况下）
    match(TokenType::SEMICOLON);
    
    return std::make_shared<StatementNode>(expr, expr->getLocation());
}

std::shared_ptr<ASTNode> Parser::parseVariableDeclaration() {
    auto location = previous_->getLocation();
    
    VariableDeclarationNode::DeclarationType declType;
    if (previous_->getType() == TokenType::KEYWORD_CONST) {
        declType = VariableDeclarationNode::DeclarationType::CONST;
    } else if (previous_->getType() == TokenType::KEYWORD_LET) {
        declType = VariableDeclarationNode::DeclarationType::LET;
    } else {
        declType = VariableDeclarationNode::DeclarationType::VAR;
    }
    
    std::string name = parseIdentifier();
    
    auto varDecl = std::make_shared<VariableDeclarationNode>(declType, name, location);
    
    // 初始化器是可选的
    if (match(TokenType::EQUAL)) {
        varDecl->setInitializer(parseAssignment());
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    
    return varDecl;
}

std::shared_ptr<ASTNode> Parser::parseModuleBlock() {
    auto location = current_->getLocation();
    consume(TokenType::KEYWORD_MODULE, "Expected 'module'");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'module'");
    
    auto moduleNode = std::make_shared<ModuleNode>(location);
    
    enterState(StateType::IN_MODULE_BLOCK);
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match(TokenType::KEYWORD_LOAD)) {
            consume(TokenType::COLON, "Expected ':' after 'load'");
            
            // 解析加载列表
            do {
                if (check(TokenType::STRING_LITERAL)) {
                    moduleNode->addLoadItem(parseString());
                } else if (check(TokenType::UNQUOTED_LITERAL) || check(TokenType::IDENTIFIER)) {
                    moduleNode->addLoadItem(current_->getLexeme());
                    advance();
                }
            } while (match(TokenType::COMMA));
            
            match(TokenType::SEMICOLON);  // 分号是可选的
        } else {
            error("Expected 'load' in module block");
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after module content");
    
    exitState();
    
    return moduleNode;
}

std::shared_ptr<ASTNode> Parser::parseEnhancedSelector() {
    // 已经消费了 {{
    // auto location = previous_->getLocation(); // TODO: 使用location
    
    std::string selector;
    if (check(TokenType::SELECTOR_CLASS) || 
        check(TokenType::SELECTOR_ID) || 
        check(TokenType::SELECTOR_TAG) ||
        check(TokenType::SELECTOR_COMPOUND) ||
        check(TokenType::SELECTOR_REF)) {
        selector = current_->getLexeme();
        advance();
    } else {
        error("Expected selector in {{}}");
    }
    
    // TODO: 实现EnhancedSelectorNode后启用
    // EnhancedSelectorNode::SelectorType type = determineSelectorType(selector);
    // auto selectorNode = std::make_shared<EnhancedSelectorNode>(selector, type, location);
    std::shared_ptr<ASTNode> selectorNode; // 临时占位
    
    // 检查索引访问 {{button[0]}}
    if (match(TokenType::LEFT_BRACKET)) {
        if (check(TokenType::NUMBER_LITERAL)) {
            auto indexToken = current_;
            advance();
            // TODO: 实现setIndex方法
            // selectorNode->setIndex(std::get<int64_t>(indexToken->getValue()));
        }
        consume(TokenType::RIGHT_BRACKET, "Expected ']' after index");
    }
    
    consume(TokenType::DOUBLE_RIGHT_BRACE, "Expected '}}' after selector");
    
    return selectorNode;
}

std::shared_ptr<ASTNode> Parser::parseExpression() {
    return parseAssignment();
}

std::shared_ptr<ASTNode> Parser::parseAssignment() {
    auto expr = parseOr();
    
    if (match(TokenType::EQUAL)) {
        auto right = parseAssignment();
        // 创建赋值表达式节点
        return std::make_shared<BinaryExpressionNode>(
            BinaryExpressionNode::Operator::EQUAL,
            expr, right, expr->getLocation());
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseOr() {
    auto expr = parseAnd();
    
    while (match(TokenType::OR)) {
        auto op = BinaryExpressionNode::Operator::OR;
        auto right = parseAnd();
        expr = std::make_shared<BinaryExpressionNode>(op, expr, right, expr->getLocation());
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseAnd() {
    auto expr = parseEquality();
    
    while (match(TokenType::AND)) {
        auto op = BinaryExpressionNode::Operator::AND;
        auto right = parseEquality();
        expr = std::make_shared<BinaryExpressionNode>(op, expr, right, expr->getLocation());
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseEquality() {
    auto expr = parseComparison();
    
    while (match({TokenType::EQUAL_EQUAL, TokenType::NOT_EQUAL})) {
        auto op = previous_->getType() == TokenType::EQUAL_EQUAL ?
                  BinaryExpressionNode::Operator::EQUAL :
                  BinaryExpressionNode::Operator::NOT_EQUAL;
        auto right = parseComparison();
        expr = std::make_shared<BinaryExpressionNode>(op, expr, right, expr->getLocation());
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseComparison() {
    auto expr = parseAddition();
    
    while (match({TokenType::LESS_THAN, TokenType::GREATER_THAN,
                  TokenType::LESS_EQUAL, TokenType::GREATER_EQUAL})) {
        BinaryExpressionNode::Operator op;
        switch (previous_->getType()) {
            case TokenType::LESS_THAN: op = BinaryExpressionNode::Operator::LESS_THAN; break;
            case TokenType::GREATER_THAN: op = BinaryExpressionNode::Operator::GREATER_THAN; break;
            case TokenType::LESS_EQUAL: op = BinaryExpressionNode::Operator::LESS_EQUAL; break;
            case TokenType::GREATER_EQUAL: op = BinaryExpressionNode::Operator::GREATER_EQUAL; break;
            default: op = BinaryExpressionNode::Operator::LESS_THAN; break;
        }
        auto right = parseAddition();
        expr = std::make_shared<BinaryExpressionNode>(op, expr, right, expr->getLocation());
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseAddition() {
    auto expr = parseMultiplication();
    
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        auto op = previous_->getType() == TokenType::PLUS ?
                  BinaryExpressionNode::Operator::ADD :
                  BinaryExpressionNode::Operator::SUBTRACT;
        auto right = parseMultiplication();
        expr = std::make_shared<BinaryExpressionNode>(op, expr, right, expr->getLocation());
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseMultiplication() {
    auto expr = parseUnary();
    
    while (match({TokenType::MULTIPLY, TokenType::DIVIDE, TokenType::MODULO})) {
        BinaryExpressionNode::Operator op;
        switch (previous_->getType()) {
            case TokenType::MULTIPLY: op = BinaryExpressionNode::Operator::MULTIPLY; break;
            case TokenType::DIVIDE: op = BinaryExpressionNode::Operator::DIVIDE; break;
            case TokenType::MODULO: op = BinaryExpressionNode::Operator::MODULO; break;
            default: op = BinaryExpressionNode::Operator::MULTIPLY; break;
        }
        auto right = parseUnary();
        expr = std::make_shared<BinaryExpressionNode>(op, expr, right, expr->getLocation());
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseUnary() {
    if (match({TokenType::NOT, TokenType::MINUS, TokenType::PLUS})) {
        UnaryExpressionNode::Operator op;
        switch (previous_->getType()) {
            case TokenType::NOT: op = UnaryExpressionNode::Operator::NOT; break;
            case TokenType::MINUS: op = UnaryExpressionNode::Operator::MINUS; break;
            case TokenType::PLUS: op = UnaryExpressionNode::Operator::PLUS; break;
            default: op = UnaryExpressionNode::Operator::NOT; break;
        }
        auto operand = parseUnary();
        return std::make_shared<UnaryExpressionNode>(op, operand, previous_->getLocation());
    }
    
    return parsePostfix();
}

std::shared_ptr<ASTNode> Parser::parsePostfix() {
    auto expr = parseCall();
    
    // 处理 -> 和 &-> 操作符
    while (true) {
        if (match(TokenType::ARROW)) {
            // 箭头访问
            auto property = parseMember();
            expr = std::make_shared<ArrowAccessNode>(expr, property, expr->getLocation());
        } else if (match(TokenType::EVENT_BIND)) {
            // 事件绑定
            std::string event = parseIdentifier();
            consume(TokenType::LEFT_BRACE, "Expected '{' after event name");
            auto handler = parseExpression();
            consume(TokenType::RIGHT_BRACE, "Expected '}' after event handler");
            
            expr = std::make_shared<EventBindingNode>(expr, event, handler, expr->getLocation());
        } else {
            break;
        }
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseCall() {
    auto expr = parseMember();
    
    while (match(TokenType::LEFT_PAREN)) {
        auto callExpr = std::make_shared<CallExpressionNode>(expr, expr->getLocation());
        
        // 解析参数列表
        if (!check(TokenType::RIGHT_PAREN)) {
            do {
                callExpr->addArgument(parseExpression());
            } while (match(TokenType::COMMA));
        }
        
        consume(TokenType::RIGHT_PAREN, "Expected ')' after arguments");
        
        expr = callExpr;
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parseMember() {
    auto expr = parsePrimary();
    
    while (match(TokenType::DOT)) {
        std::string property = parseIdentifier();
        auto propNode = std::make_shared<IdentifierNode>(property, previous_->getLocation());
        expr = std::make_shared<BinaryExpressionNode>(
            BinaryExpressionNode::Operator::DOT,
            expr, propNode, expr->getLocation());
    }
    
    return expr;
}

std::shared_ptr<ASTNode> Parser::parsePrimary() {
    // 增强选择器
    if (match(TokenType::DOUBLE_LEFT_BRACE)) {
        return parseEnhancedSelector();
    }
    
    // CHTL JS函数
    if (check(TokenType::KEYWORD_LISTEN)) {
        return parseListenBlock();
    }
    
    if (check(TokenType::KEYWORD_DELEGATE)) {
        return parseDelegateBlock();
    }
    
    if (check(TokenType::KEYWORD_ANIMATE)) {
        return parseAnimateBlock();
    }
    
    if (check(TokenType::KEYWORD_INEVERAWAY)) {
        return parseINeverAway();
    }
    
    // 字面量
    if (check(TokenType::STRING_LITERAL)) {
        auto value = current_->getValue();
        advance();
        return std::make_shared<LiteralNode>(
            LiteralNode::LiteralType::STRING, value, previous_->getLocation());
    }
    
    if (check(TokenType::NUMBER_LITERAL)) {
        auto value = current_->getValue();
        advance();
        return std::make_shared<LiteralNode>(
            LiteralNode::LiteralType::NUMBER, value, previous_->getLocation());
    }
    
    if (check(TokenType::BOOLEAN_LITERAL)) {
        auto value = current_->getValue();
        advance();
        return std::make_shared<LiteralNode>(
            LiteralNode::LiteralType::BOOLEAN, value, previous_->getLocation());
    }
    
    if (check(TokenType::NULL_LITERAL)) {
        advance();
        return std::make_shared<LiteralNode>(
            LiteralNode::LiteralType::NULL_VALUE, std::monostate{}, previous_->getLocation());
    }
    
    // 标识符
    if (check(TokenType::IDENTIFIER)) {
        return std::make_shared<IdentifierNode>(parseIdentifier(), previous_->getLocation());
    }
    
    // 括号表达式
    if (match(TokenType::LEFT_PAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    }
    
    // 对象字面量
    if (check(TokenType::LEFT_BRACE)) {
        return parseObjectLiteral();
    }
    
    // 数组字面量
    if (check(TokenType::LEFT_BRACKET)) {
        return parseArrayLiteral();
    }
    
    // 函数表达式
    if (check(TokenType::KEYWORD_FUNCTION)) {
        return parseFunctionExpression();
    }
    
    error("Expected expression");
    advance();
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::parseListenBlock() {
    auto location = current_->getLocation();
    consume(TokenType::KEYWORD_LISTEN, "Expected 'listen'");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'listen'");
    
    auto listenNode = std::make_shared<ListenNode>(location);
    
    enterState(StateType::IN_LISTEN_BLOCK);
    
    // 解析事件处理器映射
    auto handlers = parseKeyValuePairs();
    for (const auto& [event, handler] : handlers) {
        listenNode->addEventHandler(event, handler);
    }
    
    exitState();
    
    return listenNode;
}

std::shared_ptr<ASTNode> Parser::parseObjectLiteral() {
    auto location = current_->getLocation();
    consume(TokenType::LEFT_BRACE, "Expected '{'");
    
    auto objNode = std::make_shared<ObjectLiteralNode>(location);
    
    enterState(StateType::IN_OBJECT_LITERAL);
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        std::string key = parseIdentifier();
        consume(TokenType::COLON, "Expected ':' after property key");
        auto value = parseExpression();
        
        objNode->addProperty(key, value);
        
        if (!match(TokenType::COMMA)) {
            break;
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after object literal");
    
    exitState();
    
    return objNode;
}

std::string Parser::parseIdentifier() {
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected identifier");
        return "";
    }
    
    std::string id = current_->getLexeme();
    advance();
    return id;
}

std::string Parser::parseString() {
    if (!check(TokenType::STRING_LITERAL)) {
        error("Expected string literal");
        return "";
    }
    
    std::string str = std::get<std::string>(current_->getValue());
    advance();
    return str;
}

// TODO: 实现EnhancedSelectorNode后启用
/*
EnhancedSelectorNode::SelectorType Parser::determineSelectorType(const std::string& selector) {
    if (selector.empty()) return EnhancedSelectorNode::SelectorType::TAG;
    
    if (selector == "&") return EnhancedSelectorNode::SelectorType::REFERENCE;
    if (selector[0] == '.') return EnhancedSelectorNode::SelectorType::CLASS;
    if (selector[0] == '#') return EnhancedSelectorNode::SelectorType::ID;
    if (selector.find(' ') != std::string::npos || 
        selector.find('.') != std::string::npos ||
        selector.find('#') != std::string::npos) {
        return EnhancedSelectorNode::SelectorType::COMPOUND;
    }
    
    return EnhancedSelectorNode::SelectorType::TAG;
}
*/

void Parser::enterState(StateType state) {
    context_->getStateManager().pushState(state);
}

void Parser::exitState() {
    context_->getStateManager().popState();
}

bool Parser::isInState(StateType state) const {
    return context_->getStateManager().isInState(state);
}

// 其他方法的简化实现
std::shared_ptr<ASTNode> Parser::parseDelegateBlock() { return nullptr; }
std::shared_ptr<ASTNode> Parser::parseAnimateBlock() { return nullptr; }
std::shared_ptr<ASTNode> Parser::parseVirtualObject() { return nullptr; }
std::shared_ptr<ASTNode> Parser::parseINeverAway() { return nullptr; }
std::shared_ptr<ASTNode> Parser::parseFunctionDeclaration() { return nullptr; }
std::shared_ptr<ASTNode> Parser::parseIfStatement() { return nullptr; }
std::shared_ptr<ASTNode> Parser::parseForStatement() { return nullptr; }
std::shared_ptr<ASTNode> Parser::parseWhileStatement() { return nullptr; }
std::shared_ptr<ASTNode> Parser::parseReturnStatement() { return nullptr; }
std::shared_ptr<ASTNode> Parser::parseArrayLiteral() { return nullptr; }
std::shared_ptr<ASTNode> Parser::parseFunctionExpression() { return nullptr; }
std::unordered_map<std::string, std::shared_ptr<ASTNode>> Parser::parseKeyValuePairs() { return {}; }

} // namespace CHTLJS