#include "CHTLJSParser.h"
#include <sstream>
#include <algorithm>

namespace CHTLJS {

CHTLJSParser::CHTLJSParser(std::shared_ptr<Lexer> lexer, std::shared_ptr<GlobalMap> globalMap)
    : lexer(lexer), globalMap(globalMap), debugMode(false) {
    context = std::make_shared<CHTLJSContext>();
    state = std::make_shared<CHTLJSState>();
    Consume(); // 读取第一个token
}

CHTLJSParser::~CHTLJSParser() = default;

NodePtr CHTLJSParser::Parse() {
    return ParseProgram();
}

void CHTLJSParser::ReportError(const std::string& message) {
    std::stringstream ss;
    ss << "CHTL JS解析错误 [" << currentToken.line << ":" << currentToken.column << "]: " << message;
    errors.push_back(ss.str());
    context->AddError(message, currentToken.line, currentToken.column);
}

void CHTLJSParser::Consume() {
    currentToken = lexer->NextToken();
    
    // 跳过空白和换行
    while (currentToken.type == TokenType::WHITESPACE || 
           currentToken.type == TokenType::NEWLINE) {
        currentToken = lexer->NextToken();
    }
    
    if (debugMode) {
        std::cout << "Consumed: " << currentToken.ToString() << std::endl;
    }
}

Token CHTLJSParser::Peek(int offset) {
    return lexer->PeekToken(offset);
}

bool CHTLJSParser::Match(TokenType type) {
    if (currentToken.type == type) {
        Consume();
        return true;
    }
    return false;
}

bool CHTLJSParser::MatchAny(std::initializer_list<TokenType> types) {
    for (auto type : types) {
        if (currentToken.type == type) {
            Consume();
            return true;
        }
    }
    return false;
}

void CHTLJSParser::Expect(TokenType type, const std::string& message) {
    if (currentToken.type != type) {
        ReportError(message);
    } else {
        Consume();
    }
}

NodePtr CHTLJSParser::ParseProgram() {
    auto program = std::make_shared<ProgramNode>();
    
    while (currentToken.type != TokenType::EOF_TOKEN) {
        // 跳过注释
        if (currentToken.type == TokenType::SINGLE_COMMENT || 
            currentToken.type == TokenType::MULTI_COMMENT) {
            Consume();
            continue;
        }
        
        auto statement = ParseStatement();
        if (statement) {
            program->AddChild(statement);
        }
        
        // 错误恢复
        if (HasErrors() && errors.size() > 10) {
            break;
        }
    }
    
    return program;
}

NodePtr CHTLJSParser::ParseStatement() {
    StateGuard guard(state, ParseState::BLOCK);
    
    // vir声明
    if (currentToken.type == TokenType::VIR) {
        return ParseVirDeclaration();
    }
    
    // 变量声明
    if (currentToken.type == TokenType::CONST ||
        currentToken.type == TokenType::LET ||
        currentToken.type == TokenType::VAR) {
        return ParseVariableDeclaration();
    }
    
    // 函数声明
    if (currentToken.type == TokenType::FUNCTION) {
        return ParseFunctionDeclaration();
    }
    
    // 控制流语句
    if (currentToken.type == TokenType::IF) {
        return ParseIfStatement();
    }
    
    if (currentToken.type == TokenType::FOR) {
        return ParseForStatement();
    }
    
    if (currentToken.type == TokenType::WHILE) {
        return ParseWhileStatement();
    }
    
    if (currentToken.type == TokenType::RETURN) {
        return ParseReturnStatement();
    }
    
    // 块语句
    if (currentToken.type == TokenType::LEFT_BRACE) {
        return ParseBlockStatement();
    }
    
    // 表达式语句
    return ParseExpressionStatement();
}

NodePtr CHTLJSParser::ParseExpression() {
    return ParseAssignmentExpression();
}

NodePtr CHTLJSParser::ParseEnhancedSelector() {
    StateGuard guard(state, ParseState::ENHANCED_SELECTOR);
    context->EnterEnhancedSelector();
    
    Expect(TokenType::DOUBLE_LEFT_BRACE, "期望 {{");
    
    std::string selector;
    
    // 收集选择器内容，直到遇到 }}
    while (currentToken.type != TokenType::DOUBLE_RIGHT_BRACE && 
           currentToken.type != TokenType::EOF_TOKEN) {
        selector += currentToken.value;
        Consume();
    }
    
    Expect(TokenType::DOUBLE_RIGHT_BRACE, "期望 }}");
    
    context->ExitEnhancedSelector();
    
    // 去除首尾空格
    selector.erase(0, selector.find_first_not_of(" \t"));
    selector.erase(selector.find_last_not_of(" \t") + 1);
    
    auto node = std::make_shared<EnhancedSelectorNode>(selector);
    node->SetPosition(currentToken.line, currentToken.column);
    
    // 缓存选择器
    if (!globalMap->HasCachedSelector(selector)) {
        globalMap->CacheSelector(selector, node->GenerateJS());
    }
    
    return node;
}

NodePtr CHTLJSParser::ParseArrowAccess(NodePtr object) {
    Consume(); // 消费 ->
    
    std::string property = currentToken.value;
    Expect(TokenType::IDENTIFIER, "期望属性名");
    
    auto node = std::make_shared<ArrowAccessNode>(object, property);
    node->SetPosition(currentToken.line, currentToken.column);
    
    // 处理虚对象属性访问
    if (state->IsInVirtualObject()) {
        std::string virName = state->GetCurrentVirtualObject();
        return ResolveVirtualObjectProperty(virName, property);
    }
    
    return node;
}

NodePtr CHTLJSParser::ParseVirDeclaration() {
    StateGuard guard(state, ParseState::VIR_DECLARATION);
    
    Consume(); // 消费 vir
    
    std::string name = currentToken.value;
    Expect(TokenType::IDENTIFIER, "期望虚对象名称");
    
    state->EnterVirtualObject(name);
    
    Expect(TokenType::EQUALS, "期望 =");
    
    auto value = ParseExpression();
    
    state->ExitVirtualObject();
    
    // 处理虚对象
    ProcessVirtualObject(name, value);
    
    // 虚对象声明不生成运行时代码
    auto node = std::make_shared<VirDeclarationNode>(name, value);
    node->SetPosition(currentToken.line, currentToken.column);
    
    Match(TokenType::SEMICOLON); // 可选分号
    
    return node;
}

NodePtr CHTLJSParser::ParseCHTLJSFunction(const std::string& name) {
    StateGuard guard(state, ParseState::CHTLJS_FUNCTION);
    state->SetCurrentCHTLJSFunction(name);
    context->EnterCHTLJSFunction(name);
    
    auto node = std::make_shared<CHTLJSFunctionNode>(name);
    node->SetPosition(currentToken.line, currentToken.column);
    
    Expect(TokenType::LEFT_BRACE, "期望 {");
    
    // 解析函数内容
    if (name == "listen") {
        // 解析事件监听器映射
        while (currentToken.type != TokenType::RIGHT_BRACE && 
               currentToken.type != TokenType::EOF_TOKEN) {
            std::string event = currentToken.value;
            Expect(TokenType::IDENTIFIER, "期望事件名");
            
            Expect(TokenType::COLON, "期望 :");
            
            auto handler = ParseExpression();
            node->AddProperty(event, handler);
            
            if (currentToken.type == TokenType::COMMA) {
                Consume();
            }
        }
    } else if (name == "delegate") {
        // 解析事件委托
        while (currentToken.type != TokenType::RIGHT_BRACE && 
               currentToken.type != TokenType::EOF_TOKEN) {
            std::string key = currentToken.value;
            
            if (key == "target") {
                Consume();
                Expect(TokenType::COLON, "期望 :");
                auto target = ParseExpression();
                node->AddProperty("target", target);
            } else {
                // 事件处理器
                Consume();
                Expect(TokenType::COLON, "期望 :");
                auto handler = ParseExpression();
                node->AddProperty(key, handler);
            }
            
            if (currentToken.type == TokenType::COMMA) {
                Consume();
            }
        }
    } else if (name == "animate") {
        // 解析动画配置
        while (currentToken.type != TokenType::RIGHT_BRACE && 
               currentToken.type != TokenType::EOF_TOKEN) {
            std::string key = currentToken.value;
            Consume();
            
            Expect(TokenType::COLON, "期望 :");
            
            if (key == "begin" || key == "end" || key == "when") {
                // CSS块
                auto cssBlock = ParseObjectLiteral();
                node->AddProperty(key, cssBlock);
            } else {
                // 其他属性
                auto value = ParseExpression();
                node->AddProperty(key, value);
            }
            
            if (currentToken.type == TokenType::COMMA) {
                Consume();
            }
        }
    }
    
    Expect(TokenType::RIGHT_BRACE, "期望 }");
    
    context->ExitCHTLJSFunction();
    
    return node;
}

NodePtr CHTLJSParser::ParseEventBinding(NodePtr target) {
    StateGuard guard(state, ParseState::EVENT_BINDING);
    
    Consume(); // 消费 &->
    
    std::string event = currentToken.value;
    Expect(TokenType::IDENTIFIER, "期望事件名");
    
    Expect(TokenType::LEFT_BRACE, "期望 {");
    
    // 解析事件处理器（可以是语句块）
    auto handler = ParseBlockStatement();
    
    auto node = std::make_shared<EventBindingNode>(target, event, handler);
    node->SetPosition(currentToken.line, currentToken.column);
    
    return node;
}

NodePtr CHTLJSParser::ParsePrimaryExpression() {
    // 增强选择器
    if (currentToken.type == TokenType::DOUBLE_LEFT_BRACE) {
        return ParseEnhancedSelector();
    }
    
    // 字面量
    if (currentToken.type == TokenType::STRING_LITERAL ||
        currentToken.type == TokenType::NUMBER ||
        currentToken.type == TokenType::BOOLEAN ||
        currentToken.type == TokenType::NULL_LITERAL ||
        currentToken.type == TokenType::UNDEFINED) {
        return ParseLiteral();
    }
    
    // 标识符或CHTL JS函数
    if (currentToken.type == TokenType::IDENTIFIER) {
        std::string name = currentToken.value;
        Consume();
        
        // 检查是否是CHTL JS函数
        if (name == "listen" || name == "delegate" || name == "animate") {
            return ParseCHTLJSFunction(name);
        }
        
        auto node = std::make_shared<IdentifierNode>(name);
        node->SetPosition(currentToken.line, currentToken.column);
        return node;
    }
    
    // 括号表达式
    if (currentToken.type == TokenType::LEFT_PAREN) {
        Consume();
        auto expr = ParseExpression();
        Expect(TokenType::RIGHT_PAREN, "期望 )");
        return expr;
    }
    
    // 对象字面量
    if (currentToken.type == TokenType::LEFT_BRACE) {
        return ParseObjectLiteral();
    }
    
    // 数组字面量
    if (currentToken.type == TokenType::LEFT_BRACKET) {
        return ParseArrayLiteral();
    }
    
    // 函数表达式
    if (currentToken.type == TokenType::FUNCTION) {
        return ParseFunctionExpression();
    }
    
    ReportError("期望表达式");
    return nullptr;
}

NodePtr CHTLJSParser::ParsePostfixExpression() {
    auto expr = ParsePrimaryExpression();
    
    while (true) {
        // 箭头访问
        if (currentToken.type == TokenType::ARROW) {
            expr = ParseArrowAccess(expr);
        }
        // 成员访问
        else if (currentToken.type == TokenType::DOT) {
            Consume();
            std::string property = currentToken.value;
            Expect(TokenType::IDENTIFIER, "期望属性名");
            
            auto access = std::make_shared<ArrowAccessNode>(expr, property);
            access->SetPosition(currentToken.line, currentToken.column);
            expr = access;
        }
        // 函数调用
        else if (currentToken.type == TokenType::LEFT_PAREN) {
            auto call = std::make_shared<FunctionCallNode>(expr);
            call->SetPosition(currentToken.line, currentToken.column);
            
            Consume();
            
            // 解析参数
            while (currentToken.type != TokenType::RIGHT_PAREN && 
                   currentToken.type != TokenType::EOF_TOKEN) {
                call->AddArgument(ParseExpression());
                
                if (currentToken.type == TokenType::COMMA) {
                    Consume();
                } else {
                    break;
                }
            }
            
            Expect(TokenType::RIGHT_PAREN, "期望 )");
            expr = call;
        }
        // 事件绑定
        else if (currentToken.type == TokenType::AMPERSAND_ARROW) {
            expr = ParseEventBinding(expr);
        }
        else {
            break;
        }
    }
    
    return expr;
}

NodePtr CHTLJSParser::ParseBinaryExpression(int minPrecedence) {
    auto left = ParsePostfixExpression();
    
    while (IsBinaryOperator(currentToken.type)) {
        int precedence = GetOperatorPrecedence(currentToken.type);
        if (precedence < minPrecedence) {
            break;
        }
        
        std::string op = currentToken.value;
        TokenType opType = currentToken.type;
        Consume();
        
        auto right = ParseBinaryExpression(precedence + 1);
        
        auto binary = std::make_shared<BinaryOpNode>(op, left, right);
        binary->SetPosition(currentToken.line, currentToken.column);
        left = binary;
    }
    
    return left;
}

NodePtr CHTLJSParser::ParseAssignmentExpression() {
    auto expr = ParseBinaryExpression();
    
    if (IsAssignmentOperator(currentToken.type)) {
        std::string op = currentToken.value;
        Consume();
        
        auto right = ParseAssignmentExpression();
        
        auto assign = std::make_shared<BinaryOpNode>(op, expr, right);
        assign->SetPosition(currentToken.line, currentToken.column);
        return assign;
    }
    
    return expr;
}

NodePtr CHTLJSParser::ParseVariableDeclaration() {
    std::string declType = currentToken.value;
    Consume();
    
    auto decl = std::make_shared<StatementNode>();
    decl->SetPosition(currentToken.line, currentToken.column);
    
    // 解析变量名和初始值
    do {
        std::string name = currentToken.value;
        Expect(TokenType::IDENTIFIER, "期望变量名");
        
        // 声明变量
        context->DeclareVariable(name, declType, currentToken.line, currentToken.column);
        
        NodePtr init = nullptr;
        if (currentToken.type == TokenType::EQUALS) {
            Consume();
            init = ParseExpression();
        }
        
        // TODO: 创建变量声明节点
        
    } while (Match(TokenType::COMMA));
    
    Match(TokenType::SEMICOLON);
    
    return decl;
}

NodePtr CHTLJSParser::ParseBlockStatement() {
    context->PushScope(ScopeType::BLOCK);
    
    auto block = std::make_shared<StatementNode>();
    block->SetPosition(currentToken.line, currentToken.column);
    
    Expect(TokenType::LEFT_BRACE, "期望 {");
    
    while (currentToken.type != TokenType::RIGHT_BRACE && 
           currentToken.type != TokenType::EOF_TOKEN) {
        auto stmt = ParseStatement();
        if (stmt) {
            block->AddChild(stmt);
        }
    }
    
    Expect(TokenType::RIGHT_BRACE, "期望 }");
    
    context->PopScope();
    
    return block;
}

NodePtr CHTLJSParser::ParseLiteral() {
    auto node = std::make_shared<LiteralNode>(currentToken.value, LiteralNode::LiteralType::STRING);
    node->SetPosition(currentToken.line, currentToken.column);
    
    switch (currentToken.type) {
        case TokenType::STRING_LITERAL:
            node = std::make_shared<LiteralNode>(currentToken.value, LiteralNode::LiteralType::STRING);
            break;
        case TokenType::NUMBER:
            node = std::make_shared<LiteralNode>(currentToken.value, LiteralNode::LiteralType::NUMBER);
            break;
        case TokenType::BOOLEAN:
            node = std::make_shared<LiteralNode>(currentToken.value, LiteralNode::LiteralType::BOOLEAN);
            break;
        case TokenType::NULL_LITERAL:
            node = std::make_shared<LiteralNode>("null", LiteralNode::LiteralType::NULL_TYPE);
            break;
        case TokenType::UNDEFINED:
            node = std::make_shared<LiteralNode>("undefined", LiteralNode::LiteralType::UNDEFINED);
            break;
    }
    
    Consume();
    return node;
}

NodePtr CHTLJSParser::ParseObjectLiteral() {
    auto obj = std::make_shared<ObjectLiteralNode>();
    obj->SetPosition(currentToken.line, currentToken.column);
    
    Expect(TokenType::LEFT_BRACE, "期望 {");
    
    while (currentToken.type != TokenType::RIGHT_BRACE && 
           currentToken.type != TokenType::EOF_TOKEN) {
        std::string key;
        
        // 属性名可以是标识符或字符串
        if (currentToken.type == TokenType::IDENTIFIER) {
            key = currentToken.value;
            Consume();
        } else if (currentToken.type == TokenType::STRING_LITERAL) {
            key = currentToken.value;
            Consume();
        } else {
            ReportError("期望属性名");
            break;
        }
        
        Expect(TokenType::COLON, "期望 :");
        
        auto value = ParseExpression();
        obj->AddProperty(key, value);
        
        if (currentToken.type == TokenType::COMMA) {
            Consume();
        } else {
            break;
        }
    }
    
    Expect(TokenType::RIGHT_BRACE, "期望 }");
    
    return obj;
}

// 辅助方法实现
bool CHTLJSParser::IsBinaryOperator(TokenType type) {
    return type == TokenType::PLUS || type == TokenType::MINUS ||
           type == TokenType::STAR || type == TokenType::SLASH ||
           type == TokenType::PERCENT || type == TokenType::DOUBLE_STAR;
}

bool CHTLJSParser::IsAssignmentOperator(TokenType type) {
    return type == TokenType::EQUALS;
}

int CHTLJSParser::GetOperatorPrecedence(TokenType type) {
    switch (type) {
        case TokenType::DOUBLE_STAR:
            return 10;
        case TokenType::STAR:
        case TokenType::SLASH:
        case TokenType::PERCENT:
            return 9;
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 8;
        default:
            return 0;
    }
}

void CHTLJSParser::ProcessVirtualObject(const std::string& name, NodePtr value) {
    auto virObj = std::make_shared<VirtualObject>(name);
    
    // 如果值是CHTL JS函数，提取其属性
    if (value->GetType() == NodeType::CHTLJS_FUNCTION) {
        auto funcNode = std::static_pointer_cast<CHTLJSFunctionNode>(value);
        
        // 遍历函数的所有属性
        // TODO: 需要CHTLJSFunctionNode提供属性遍历接口
    }
    
    globalMap->AddVirtualObject(name, virObj);
}

NodePtr CHTLJSParser::ResolveVirtualObjectProperty(const std::string& name, const std::string& property) {
    auto virObj = globalMap->GetVirtualObject(name);
    if (virObj && virObj->HasProperty(property)) {
        if (virObj->IsFunction(property)) {
            // 返回函数引用
            auto funcRef = std::make_shared<IdentifierNode>(virObj->GetProperty(property));
            funcRef->SetPosition(currentToken.line, currentToken.column);
            return funcRef;
        } else {
            // 返回属性值
            auto propValue = std::make_shared<LiteralNode>(
                virObj->GetProperty(property), 
                LiteralNode::LiteralType::STRING
            );
            propValue->SetPosition(currentToken.line, currentToken.column);
            return propValue;
        }
    }
    
    ReportError("虚对象 '" + name + "' 没有属性 '" + property + "'");
    return nullptr;
}

// 其他方法的占位实现
NodePtr CHTLJSParser::ParseFunctionDeclaration() {
    // TODO: 实现函数声明解析
    return nullptr;
}

NodePtr CHTLJSParser::ParseIfStatement() {
    // TODO: 实现if语句解析
    return nullptr;
}

NodePtr CHTLJSParser::ParseForStatement() {
    // TODO: 实现for循环解析
    return nullptr;
}

NodePtr CHTLJSParser::ParseWhileStatement() {
    // TODO: 实现while循环解析
    return nullptr;
}

NodePtr CHTLJSParser::ParseReturnStatement() {
    // TODO: 实现return语句解析
    return nullptr;
}

NodePtr CHTLJSParser::ParseExpressionStatement() {
    auto expr = ParseExpression();
    Match(TokenType::SEMICOLON);
    return expr;
}

NodePtr CHTLJSParser::ParseArrayLiteral() {
    // TODO: 实现数组字面量解析
    return nullptr;
}

NodePtr CHTLJSParser::ParseFunctionExpression() {
    // TODO: 实现函数表达式解析
    return nullptr;
}

} // namespace CHTLJS