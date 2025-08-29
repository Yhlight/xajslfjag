#include "Parser.h"
#include "../CHTLJSNode/CHTLJSNodes.h"
#include "../CHTLJSNode/ModuleNode.h"
#include "../../CHTL/CHTLLexer/GlobalMap.h"
#include <regex>
#include <algorithm>

namespace CHTLJS {

// CHTLJSParser实现
CHTLJSParser::CHTLJSParser(std::unique_ptr<CHTLJSLexer> lexer, const CHTLJSParserConfig& config)
    : lexer(std::move(lexer)), config(config), currentDepth(0) {
    errors.clear();
    warnings.clear();
    advance(); // 获取第一个token
}

std::unique_ptr<CHTL::BaseNode> CHTLJSParser::parse() {
    try {
        return parseDocument();
    } catch (const std::exception& e) {
        reportError("解析异常: " + CHTL::String(e.what()));
        return nullptr;
    }
}

std::unique_ptr<CHTL::BaseNode> CHTLJSParser::parseDocument() {
    auto document = CHTL::NodeFactory::createDocumentNode();
    
    while (!isAtEnd()) {
        if (auto node = parseStatement()) {
            document->addChild(std::move(node));
        } else {
            if (!recover()) break;
        }
    }
    
    return document;
}

std::unique_ptr<CHTL::BaseNode> CHTLJSParser::parseStatement() {
    // 跳过空白和注释
    skipWhitespaceAndComments();
    
    if (isAtEnd()) return nullptr;
    
    // 根据当前token解析不同的CHTL JS语句
    switch (currentToken.type) {
        case CHTL::TokenType::MODULE:
            return parseModuleBlock();
        case CHTL::TokenType::LISTEN:
            return parseListenBlock();
        case CHTL::TokenType::DELEGATE:
            return parseDelegateBlock();
        case CHTL::TokenType::ANIMATE:
            return parseAnimateBlock();
        case CHTL::TokenType::VIR:
            return parseVirtualObject();
        case CHTL::TokenType::DOUBLE_LBRACE:
            return parseEnhancedSelector();
        default:
            // 处理JavaScript语句
            return parseJavaScriptStatement();
    }
}

std::unique_ptr<CHTL::BaseNode> CHTLJSParser::parseModuleBlock() {
    CHTL::Position pos = currentToken.position;
    advance(); // 消费 'module'
    
    auto moduleNode = std::make_unique<ModuleNode>("", pos);
    
    consume(CHTL::TokenType::LBRACE, "期望 '{'");
    
    while (!check(CHTL::TokenType::RBRACE) && !isAtEnd()) {
        if (check(CHTL::TokenType::LOAD)) {
            parseModuleLoad(moduleNode.get());
        } else {
            // 解析简化语法 (./file.js)
            parseModuleSimpleSyntax(moduleNode.get());
        }
        
        // 可选的逗号
        if (check(CHTL::TokenType::COMMA)) {
            advance();
        }
    }
    
    consume(CHTL::TokenType::RBRACE, "期望 '}'");
    
    return std::unique_ptr<CHTL::BaseNode>(moduleNode.release());
}

void CHTLJSParser::parseModuleLoad(ModuleNode* moduleNode) {
    advance(); // 消费 'load'
    consume(CHTL::TokenType::COLON, "期望 ':'");
    
    CHTL::String modulePath = parseLiteral();
    moduleNode->addModule(modulePath);
}

void CHTLJSParser::parseModuleSimpleSyntax(ModuleNode* moduleNode) {
    CHTL::String modulePath = parseLiteral();
    moduleNode->addModule(modulePath);
}

std::unique_ptr<CHTL::BaseNode> CHTLJSParser::parseListenBlock() {
    CHTL::Position pos = currentToken.position;
    advance(); // 消费 'listen'
    
    auto listenNode = std::make_unique<ListenNode>("", pos);
    
    consume(CHTL::TokenType::LBRACE, "期望 '{'");
    
    while (!check(CHTL::TokenType::RBRACE) && !isAtEnd()) {
        // 解析事件处理器
        CHTL::String eventName = parseIdentifier();
        consume(CHTL::TokenType::COLON, "期望 ':'");
        
        CHTL::String handlerCode = parseJavaScriptExpression();
        listenNode->addEventHandler(eventName, handlerCode);
        
        // 可选的逗号
        if (check(CHTL::TokenType::COMMA)) {
            advance();
        }
    }
    
    consume(CHTL::TokenType::RBRACE, "期望 '}'");
    
    return std::unique_ptr<CHTL::BaseNode>(listenNode.release());
}

std::unique_ptr<CHTL::BaseNode> CHTLJSParser::parseDelegateBlock() {
    CHTL::Position pos = currentToken.position;
    advance(); // 消费 'delegate'
    
    auto delegateNode = std::make_unique<DelegateNode>("", pos);
    
    consume(CHTL::TokenType::LBRACE, "期望 '{'");
    
    while (!check(CHTL::TokenType::RBRACE) && !isAtEnd()) {
        // 解析委托选择器
        CHTL::String selector = parseLiteral();
        consume(CHTL::TokenType::COLON, "期望 ':'");
        
        // 解析事件处理器对象
        consume(CHTL::TokenType::LBRACE, "期望 '{'");
        CHTL::StringUnorderedMap handlers;
        
        while (!check(CHTL::TokenType::RBRACE) && !isAtEnd()) {
            CHTL::String eventName = parseIdentifier();
            consume(CHTL::TokenType::COLON, "期望 ':'");
            CHTL::String handlerCode = parseJavaScriptExpression();
            
            handlers[eventName] = handlerCode;
            
            if (check(CHTL::TokenType::COMMA)) {
                advance();
            }
        }
        
        consume(CHTL::TokenType::RBRACE, "期望 '}'");
        delegateNode->addDelegateHandler(selector, handlers);
        
        if (check(CHTL::TokenType::COMMA)) {
            advance();
        }
    }
    
    consume(CHTL::TokenType::RBRACE, "期望 '}'");
    
    return std::unique_ptr<CHTL::BaseNode>(delegateNode.release());
}

std::unique_ptr<CHTL::BaseNode> CHTLJSParser::parseAnimateBlock() {
    CHTL::Position pos = currentToken.position;
    advance(); // 消费 'animate'
    
    auto animateNode = std::make_unique<AnimateNode>("", pos);
    
    consume(CHTL::TokenType::LBRACE, "期望 '{'");
    
    while (!check(CHTL::TokenType::RBRACE) && !isAtEnd()) {
        CHTL::String propertyName = parseIdentifier();
        consume(CHTL::TokenType::COLON, "期望 ':'");
        CHTL::String propertyValue = parseLiteral();
        
        animateNode->setAnimationProperty(propertyName, propertyValue);
        
        if (check(CHTL::TokenType::COMMA)) {
            advance();
        }
    }
    
    consume(CHTL::TokenType::RBRACE, "期望 '}'");
    
    return std::unique_ptr<CHTL::BaseNode>(animateNode.release());
}

std::unique_ptr<CHTL::BaseNode> CHTLJSParser::parseVirtualObject() {
    CHTL::Position pos = currentToken.position;
    advance(); // 消费 'vir'
    
    CHTL::String objectName = parseIdentifier();
    consume(CHTL::TokenType::EQUALS, "期望 '='");
    
    auto virNode = std::make_unique<VirtualObjectNode>(objectName, pos);
    
    // 解析虚对象类型
    if (check(CHTL::TokenType::LISTEN)) {
        // vir obj = listen { ... }
        auto listenBlock = parseListenBlock();
        virNode->setVirtualType("listen");
        virNode->addChild(std::move(listenBlock));
    } else if (check(CHTL::TokenType::LBRACE)) {
        // vir obj = { ... }
        virNode->setVirtualType("object");
        parseVirtualObjectBody(virNode.get());
    } else {
        reportError("期望虚对象定义");
    }
    
    return std::unique_ptr<CHTL::BaseNode>(virNode.release());
}

void CHTLJSParser::parseVirtualObjectBody(VirtualObjectNode* virNode) {
    consume(CHTL::TokenType::LBRACE, "期望 '{'");
    
    while (!check(CHTL::TokenType::RBRACE) && !isAtEnd()) {
        CHTL::String propertyName = parseIdentifier();
        consume(CHTL::TokenType::COLON, "期望 ':'");
        
        CHTL::String propertyValue = parseJavaScriptExpression();
        virNode->setProperty(propertyName, propertyValue);
        
        if (check(CHTL::TokenType::COMMA)) {
            advance();
        }
    }
    
    consume(CHTL::TokenType::RBRACE, "期望 '}'");
}

std::unique_ptr<CHTL::BaseNode> CHTLJSParser::parseEnhancedSelector() {
    CHTL::Position pos = currentToken.position;
    advance(); // 消费 '{{'
    
    CHTL::String selectorContent = "";
    
    // 解析选择器内容
    while (!check(CHTL::TokenType::DOUBLE_RBRACE) && !isAtEnd()) {
        selectorContent += currentToken.value;
        advance();
    }
    
    consume(CHTL::TokenType::DOUBLE_RBRACE, "期望 '}}'");
    
    auto selectorNode = std::make_unique<EnhancedSelectorNode>(selectorContent, pos);
    
    // 解析选择器后的操作
    if (check(CHTL::TokenType::ARROW)) {
        advance(); // 消费 '->'
        parseEnhancedSelectorAction(selectorNode.get());
    } else if (check(CHTL::TokenType::EVENT_BIND)) {
        advance(); // 消费 '&->'
        parseEventBindingAction(selectorNode.get());
    }
    
    return std::unique_ptr<CHTL::BaseNode>(selectorNode.release());
}

void CHTLJSParser::parseEnhancedSelectorAction(EnhancedSelectorNode* selectorNode) {
    if (check(CHTL::TokenType::LISTEN)) {
        // {{selector}} -> listen { ... }
        auto listenBlock = parseListenBlock();
        selectorNode->setAction("listen");
        selectorNode->addChild(std::move(listenBlock));
    } else if (check(CHTL::TokenType::LBRACE)) {
        // {{selector}} -> { code }
        selectorNode->setAction("code");
        CHTL::String code = parseCodeBlock();
        selectorNode->setActionCode(code);
    } else {
        // {{selector}} -> identifier { ... }
        CHTL::String actionType = parseIdentifier();
        selectorNode->setAction(actionType);
        
        if (check(CHTL::TokenType::LBRACE)) {
            CHTL::String code = parseCodeBlock();
            selectorNode->setActionCode(code);
        }
    }
}

void CHTLJSParser::parseEventBindingAction(EnhancedSelectorNode* selectorNode) {
    // {{selector}} &-> eventType { code }
    CHTL::String eventType = parseIdentifier();
    CHTL::String eventCode = parseCodeBlock();
    
    selectorNode->setAction("event_bind");
    selectorNode->setEventType(eventType);
    selectorNode->setActionCode(eventCode);
}

std::unique_ptr<CHTL::BaseNode> CHTLJSParser::parseJavaScriptStatement() {
    // 解析标准JavaScript语句
    CHTL::String jsCode = parseJavaScriptExpression();
    
    auto jsNode = std::make_unique<CHTL::BaseNode>(CHTL::NodeType::SCRIPT, jsCode, currentToken.position);
    return jsNode;
}

CHTL::String CHTLJSParser::parseJavaScriptExpression() {
    CHTL::String expression = "";
    int braceLevel = 0;
    
    while (!isAtEnd()) {
        if (currentToken.type == CHTL::TokenType::LBRACE) {
            braceLevel++;
        } else if (currentToken.type == CHTL::TokenType::RBRACE) {
            if (braceLevel == 0) break;
            braceLevel--;
        } else if (currentToken.type == CHTL::TokenType::SEMICOLON && braceLevel == 0) {
            advance(); // 消费分号
            break;
        } else if (currentToken.type == CHTL::TokenType::COMMA && braceLevel == 0) {
            break; // 不消费逗号
        }
        
        expression += currentToken.value + " ";
        advance();
    }
    
    return expression;
}

CHTL::String CHTLJSParser::parseCodeBlock() {
    consume(CHTL::TokenType::LBRACE, "期望 '{'");
    
    CHTL::String code = "";
    int braceLevel = 1;
    
    while (braceLevel > 0 && !isAtEnd()) {
        if (currentToken.type == CHTL::TokenType::LBRACE) {
            braceLevel++;
        } else if (currentToken.type == CHTL::TokenType::RBRACE) {
            braceLevel--;
        }
        
        if (braceLevel > 0) {
            code += currentToken.value + " ";
        }
        advance();
    }
    
    return code;
}

CHTL::String CHTLJSParser::parseLiteral() {
    CHTL::String literal;
    
    switch (currentToken.type) {
        case CHTL::TokenType::LITERAL_STRING:
        case CHTL::TokenType::LITERAL_UNQUOTED:
        case CHTL::TokenType::LITERAL_NUMBER:
            literal = currentToken.value;
            advance();
            break;
        default:
            reportError("期望字面量");
            break;
    }
    
    return literal;
}

CHTL::String CHTLJSParser::parseIdentifier() {
    if (currentToken.type == CHTL::TokenType::IDENTIFIER) {
        CHTL::String identifier = currentToken.value;
        advance();
        return identifier;
    }
    
    reportError("期望标识符");
    return "";
}

// 选择器自动化处理
void CHTLJSParser::applySelectorAutomation(CHTL::BaseNode* ast) {
    if (!config.enableSelectorAutomation) return;
    
    // 遍历AST并应用选择器自动化
    applySelectorAutomationToNode(ast);
}

void CHTLJSParser::applySelectorAutomationToNode(CHTL::BaseNode* node) {
    if (!node) return;
    
    // 处理增强选择器节点
    if (node->type == CHTL::NodeType::CHTLJS_SELECTOR) {
        EnhancedSelectorNode* selectorNode = static_cast<EnhancedSelectorNode*>(node);
        processEnhancedSelector(selectorNode);
    }
    
    // 递归处理子节点
    for (const auto& child : node->children) {
        applySelectorAutomationToNode(child.get());
    }
}

void CHTLJSParser::processEnhancedSelector(EnhancedSelectorNode* selectorNode) {
    CHTL::String selector = selectorNode->getSelector();
    
    // 处理不同类型的选择器
    if (selector.front() == '.') {
        // 类选择器自动化
        processClassSelector(selectorNode, selector);
    } else if (selector.front() == '#') {
        // ID选择器自动化
        processIdSelector(selectorNode, selector);
    } else {
        // 元素选择器自动化
        processElementSelector(selectorNode, selector);
    }
}

void CHTLJSParser::processClassSelector(EnhancedSelectorNode* selectorNode, const CHTL::String& selector) {
    CHTL::String className = selector.substr(1); // 移除'.'
    
    // 检查是否需要自动添加class属性
    if (config.autoAddClassAttribute) {
        selectorNode->setAutoClass(className);
    }
    
    // 生成对应的CSS选择器
    if (config.generateCorrespondingCSS) {
        CHTL::String cssRule = selector + " {\n    /* Auto-generated for CHTL JS */\n}";
        selectorNode->setGeneratedCSS(cssRule);
    }
}

void CHTLJSParser::processIdSelector(EnhancedSelectorNode* selectorNode, const CHTL::String& selector) {
    CHTL::String idName = selector.substr(1); // 移除'#'
    
    // 检查是否需要自动添加id属性
    if (config.autoAddIdAttribute) {
        selectorNode->setAutoId(idName);
    }
    
    // 生成对应的CSS选择器
    if (config.generateCorrespondingCSS) {
        CHTL::String cssRule = selector + " {\n    /* Auto-generated for CHTL JS */\n}";
        selectorNode->setGeneratedCSS(cssRule);
    }
}

void CHTLJSParser::processElementSelector(EnhancedSelectorNode* selectorNode, const CHTL::String& selector) {
    // 验证是否是有效的HTML元素
    if (CHTL::HTMLElementMap::isHTMLElement(selector)) {
        // 生成对应的CSS选择器
        if (config.generateCorrespondingCSS) {
            CHTL::String cssRule = selector + " {\n    /* Auto-generated for CHTL JS */\n}";
            selectorNode->setGeneratedCSS(cssRule);
        }
    } else {
        reportWarning("未知的HTML元素: " + selector);
    }
}

// Token操作方法
CHTL::Token CHTLJSParser::nextToken() {
    if (lexer) {
        return lexer->nextToken();
    }
    return CHTL::Token(CHTL::TokenType::EOF_TOKEN, "", CHTL::Position{});
}

bool CHTLJSParser::match(CHTL::TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool CHTLJSParser::check(CHTL::TokenType type) const {
    return currentToken.type == type;
}

CHTL::Token CHTLJSParser::consume(CHTL::TokenType type, const CHTL::String& errorMessage) {
    if (check(type)) {
        CHTL::Token token = currentToken;
        advance();
        return token;
    }
    
    reportError(errorMessage.empty() ? "意外的Token" : errorMessage);
    return CHTL::Token(CHTL::TokenType::UNKNOWN, "", currentToken.position);
}

void CHTLJSParser::advance() {
    if (!isAtEnd()) {
        currentToken = nextToken();
    }
}

bool CHTLJSParser::isAtEnd() const {
    return currentToken.type == CHTL::TokenType::EOF_TOKEN;
}

void CHTLJSParser::skipWhitespaceAndComments() {
    while (currentToken.type == CHTL::TokenType::WHITESPACE ||
           currentToken.type == CHTL::TokenType::NEWLINE ||
           currentToken.type == CHTL::TokenType::COMMENT_LINE ||
           currentToken.type == CHTL::TokenType::COMMENT_BLOCK) {
        advance();
    }
}

bool CHTLJSParser::recover() {
    // 简化的错误恢复
    advance();
    return !isAtEnd();
}

void CHTLJSParser::reportError(const CHTL::String& message) {
    CHTLJSParseError error;
    error.message = message;
    error.position = currentToken.position;
    error.errorCode = "PARSE_ERROR";
    errors.push_back(error);
}

void CHTLJSParser::reportWarning(const CHTL::String& message) {
    warnings.push_back(message);
}

bool CHTLJSParser::hasErrors() const {
    return !errors.empty();
}

bool CHTLJSParser::hasWarnings() const {
    return !warnings.empty();
}

std::vector<CHTLJSParseError> CHTLJSParser::getErrors() const {
    return errors;
}

CHTL::StringVector CHTLJSParser::getWarnings() const {
    return warnings;
}

void CHTLJSParser::setConfig(const CHTLJSParserConfig& newConfig) {
    config = newConfig;
}

CHTLJSParserConfig CHTLJSParser::getConfig() const {
    return config;
}

} // namespace CHTLJS