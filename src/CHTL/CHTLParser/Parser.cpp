#include "Parser.h"
#include "../CHTLLexer/GlobalMap.h"
#include <iostream>

namespace CHTL {

// ParseError实现
String ParseError::toString() const {
    return "[" + std::to_string(position.line) + ":" + std::to_string(position.column) + "] " +
           errorCode + ": " + message + 
           (suggestion.empty() ? "" : " (建议: " + suggestion + ")");
}

// Parser构造函数
Parser::Parser(std::unique_ptr<Lexer> lexer, const ParserConfig& config)
    : lexer(std::move(lexer)), config(config), debugMode(false), currentDepth(0) {
    // 获取第一个token
    advance();
}

// 主解析方法
std::unique_ptr<BaseNode> Parser::parse() {
    try {
        return parseDocument();
    } catch (const std::exception& e) {
        reportError("解析异常: " + String(e.what()), "PARSE_EXCEPTION");
        return nullptr;
    }
}

std::unique_ptr<BaseNode> Parser::parseDocument() {
    auto document = NodeFactory::createDocumentNode();
    
    while (!isAtEnd()) {
        if (auto node = parseStatement()) {
            document->addChild(std::move(node));
        } else {
            // 错误恢复
            if (!recover()) {
                break;
            }
        }
    }
    
    return document;
}

std::unique_ptr<BaseNode> Parser::parseFragment() {
    return parseStatement();
}

// 语句解析
std::unique_ptr<BaseNode> Parser::parseStatement() {
    // 跳过注释和空白
    while (currentToken.type == TokenType::COMMENT_LINE || 
           currentToken.type == TokenType::COMMENT_BLOCK ||
           currentToken.type == TokenType::WHITESPACE ||
           currentToken.type == TokenType::NEWLINE) {
        if (currentToken.type == TokenType::COMMENT_LINE || 
            currentToken.type == TokenType::COMMENT_BLOCK) {
            auto comment = parseComment();
            if (comment) return comment;
        }
        advance();
    }
    
    // 根据当前token类型决定解析什么
    switch (currentToken.type) {
        case TokenType::TEXT:
            return parseText();
        case TokenType::STYLE:
            return parseStyle();
        case TokenType::SCRIPT:
            return parseScript();
        case TokenType::TEMPLATE:
            return parseTemplate();
        case TokenType::CUSTOM:
            return parseCustom();
        case TokenType::ORIGIN:
            return parseOrigin();
        case TokenType::IMPORT:
            return parseImport();
        case TokenType::CONFIGURATION:
            return parseConfiguration();
        case TokenType::NAMESPACE:
            return parseNamespace();
        case TokenType::INHERIT:
            return parseInherit();
        case TokenType::DELETE:
            return parseDelete();
        case TokenType::INSERT:
            return parseInsert();
        case TokenType::USE:
            return parseUse();
        case TokenType::HTML_ELEMENT:
        case TokenType::IDENTIFIER:
            return parseElement();
        default:
            if (HTMLElementMap::isHTMLElement(currentToken.value)) {
                return parseElement();
            }
            reportUnexpectedToken(TokenType::IDENTIFIER);
            return nullptr;
    }
}

// 元素解析
std::unique_ptr<BaseNode> Parser::parseElement() {
    String tagName = currentToken.value;
    Position pos = currentToken.position;
    advance(); // 消费元素名
    
    auto element = NodeFactory::createElementNode(tagName);
    element->position = pos;
    
    // 解析属性和内容块
    if (check(TokenType::LBRACE)) {
        advance(); // 消费 '{'
        
        // 解析块内容
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            if (isAttribute()) {
                parseElementAttributes(element.get());
            } else {
                // 解析子节点
                if (auto child = parseStatement()) {
                    element->addChild(std::move(child));
                }
            }
        }
        
        consume(TokenType::RBRACE, "期望 '}'");
    }
    
    return std::unique_ptr<BaseNode>(element.release());
}

// 属性解析
void Parser::parseElementAttributes(ElementNode* element) {
    while (isAttribute() && !isAtEnd()) {
        String attrName = currentToken.value;
        advance(); // 消费属性名
        
        // 消费 ':' 或 '='
        if (check(TokenType::COLON) || check(TokenType::EQUALS)) {
            advance();
        } else {
            reportError("期望 ':' 或 '='", "MISSING_ATTR_SEPARATOR");
        }
        
        // 解析属性值
        String attrValue = parseAttributeValue();
        element->setAttribute(attrName, attrValue);
        
        // 可选的分号
        if (check(TokenType::SEMICOLON)) {
            advance();
        }
    }
}

String Parser::parseAttributeValue() {
    String value;
    
    switch (currentToken.type) {
        case TokenType::LITERAL_STRING:
            value = currentToken.value;
            advance();
            break;
        case TokenType::LITERAL_UNQUOTED:
        case TokenType::IDENTIFIER:
        case TokenType::LITERAL_NUMBER:
            value = currentToken.value;
            advance();
            break;
        default:
            reportError("期望属性值", "MISSING_ATTR_VALUE");
            break;
    }
    
    return value;
}

// 文本节点解析
std::unique_ptr<BaseNode> Parser::parseText() {
    Position pos = currentToken.position;
    advance(); // 消费 'text'
    
    consume(TokenType::LBRACE, "期望 '{'");
    
    String textContent = parseLiteral();
    
    consume(TokenType::RBRACE, "期望 '}'");
    
    return NodeFactory::createTextNode(textContent);
}

// 样式节点解析
std::unique_ptr<BaseNode> Parser::parseStyle() {
    Position pos = currentToken.position;
    advance(); // 消费 'style'
    
    auto styleNode = NodeFactory::createStyleNode();
    styleNode->position = pos;
    
    consume(TokenType::LBRACE, "期望 '{'");
    
    // 解析样式内容
    String styleContent;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        styleContent += currentToken.value + " ";
        advance();
    }
    
    styleNode->value = styleContent;
    
    consume(TokenType::RBRACE, "期望 '}'");
    
    return styleNode;
}

// 脚本节点解析
std::unique_ptr<BaseNode> Parser::parseScript() {
    Position pos = currentToken.position;
    advance(); // 消费 'script'
    
    auto scriptNode = NodeFactory::createScriptNode();
    scriptNode->position = pos;
    
    consume(TokenType::LBRACE, "期望 '{'");
    
    // 解析脚本内容
    String scriptContent;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        scriptContent += currentToken.value + " ";
        advance();
    }
    
    scriptNode->value = scriptContent;
    
    consume(TokenType::RBRACE, "期望 '}'");
    
    return scriptNode;
}

// 模板解析
std::unique_ptr<BaseNode> Parser::parseTemplate() {
    Position pos = currentToken.position;
    advance(); // 消费 '[Template]'
    
    // 解析模板类型
    String templateType;
    if (check(TokenType::AT_STYLE)) {
        templateType = "Style";
        advance();
    } else if (check(TokenType::AT_ELEMENT)) {
        templateType = "Element";
        advance();
    } else if (check(TokenType::AT_VAR)) {
        templateType = "Var";
        advance();
    } else {
        reportError("期望模板类型 (@Style, @Element, @Var)", "MISSING_TEMPLATE_TYPE");
        return nullptr;
    }
    
    // 解析模板名称
    String templateName = consume(TokenType::IDENTIFIER, "期望模板名称").value;
    
    auto templateNode = NodeFactory::createTemplateNode(templateType, templateName);
    templateNode->position = pos;
    
    // 解析模板体
    consume(TokenType::LBRACE, "期望 '{'");
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (auto child = parseStatement()) {
            templateNode->addChild(std::move(child));
        }
    }
    
    consume(TokenType::RBRACE, "期望 '}'");
    
    return templateNode;
}

// 自定义节点解析
std::unique_ptr<BaseNode> Parser::parseCustom() {
    Position pos = currentToken.position;
    advance(); // 消费 '[Custom]'
    
    // 解析自定义类型
    String customType;
    if (check(TokenType::AT_STYLE)) {
        customType = "Style";
        advance();
    } else if (check(TokenType::AT_ELEMENT)) {
        customType = "Element";
        advance();
    } else if (check(TokenType::AT_VAR)) {
        customType = "Var";
        advance();
    } else {
        reportError("期望自定义类型 (@Style, @Element, @Var)", "MISSING_CUSTOM_TYPE");
        return nullptr;
    }
    
    // 解析自定义名称
    String customName = consume(TokenType::IDENTIFIER, "期望自定义名称").value;
    
    auto customNode = NodeFactory::createCustomNode(customType, customName);
    customNode->position = pos;
    
    // 解析自定义体
    consume(TokenType::LBRACE, "期望 '{'");
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (auto child = parseStatement()) {
            customNode->addChild(std::move(child));
        }
    }
    
    consume(TokenType::RBRACE, "期望 '}'");
    
    return customNode;
}

// 原始嵌入解析
std::unique_ptr<BaseNode> Parser::parseOrigin() {
    Position pos = currentToken.position;
    advance(); // 消费 '[Origin]'
    
    // 解析原始类型
    String originType;
    if (check(TokenType::AT_HTML)) {
        originType = "Html";
        advance();
    } else if (check(TokenType::AT_JAVASCRIPT)) {
        originType = "JavaScript";
        advance();
    } else {
        reportError("期望原始类型 (@Html, @JavaScript)", "MISSING_ORIGIN_TYPE");
        return nullptr;
    }
    
    // 可选的名称
    String originName;
    if (check(TokenType::IDENTIFIER)) {
        originName = currentToken.value;
        advance();
    }
    
    auto originNode = NodeFactory::createOriginNode(originType, originName);
    originNode->position = pos;
    
    // 解析原始内容
    consume(TokenType::LBRACE, "期望 '{'");
    
    String originContent;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        originContent += currentToken.value + " ";
        advance();
    }
    
    originNode->value = originContent;
    
    consume(TokenType::RBRACE, "期望 '}'");
    
    return originNode;
}

// 导入解析
std::unique_ptr<BaseNode> Parser::parseImport() {
    Position pos = currentToken.position;
    advance(); // 消费 '[Import]'
    
    // 解析导入类型
    String importType;
    
    if (check(TokenType::AT_HTML)) {
        importType = "Html";
        advance();
    } else if (check(TokenType::AT_JAVASCRIPT)) {
        importType = "JavaScript";
        advance();
    } else if (check(TokenType::CHTL)) {
        importType = "Chtl";
        advance();
    } else if (check(TokenType::CJMOD)) {
        importType = "CJmod";
        advance();
    } else if (check(TokenType::CONFIG)) {
        importType = "Config";
        advance();
    } else {
        reportError("期望导入类型", "MISSING_IMPORT_TYPE");
        return nullptr;
    }
    
    // 解析 from 关键字
    consume(TokenType::FROM, "期望 'from'");
    
    // 解析源路径
    String sourcePath = parseLiteral();
    
    auto importNode = NodeFactory::createImportNode(importType, sourcePath);
    importNode->position = pos;
    
    // 可选的 as 重命名
    if (check(TokenType::AS)) {
        advance();
        String alias = consume(TokenType::IDENTIFIER, "期望别名").value;
        importNode->setAttribute("alias", alias);
    }
    
    return importNode;
}

// 配置解析
std::unique_ptr<BaseNode> Parser::parseConfiguration() {
    Position pos = currentToken.position;
    advance(); // 消费 '[Configuration]'
    
    auto configNode = NodeFactory::createConfigNode();
    configNode->position = pos;
    
    // 可选的配置名称
    if (check(TokenType::CONFIG)) {
        advance();
        String configName = consume(TokenType::IDENTIFIER, "期望配置名称").value;
        configNode->setAttribute("name", configName);
    }
    
    consume(TokenType::LBRACE, "期望 '{'");
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (auto child = parseStatement()) {
            configNode->addChild(std::move(child));
        }
    }
    
    consume(TokenType::RBRACE, "期望 '}'");
    
    return configNode;
}

// 命名空间解析
std::unique_ptr<BaseNode> Parser::parseNamespace() {
    Position pos = currentToken.position;
    advance(); // 消费 '[Namespace]'
    
    String namespaceName = consume(TokenType::IDENTIFIER, "期望命名空间名称").value;
    
    auto namespaceNode = NodeFactory::createNamespaceNode(namespaceName);
    namespaceNode->position = pos;
    
    consume(TokenType::LBRACE, "期望 '{'");
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (auto child = parseStatement()) {
            namespaceNode->addChild(std::move(child));
        }
    }
    
    consume(TokenType::RBRACE, "期望 '}'");
    
    return namespaceNode;
}

// 注释解析
std::unique_ptr<BaseNode> Parser::parseComment() {
    auto commentNode = std::make_unique<BaseNode>(NodeType::COMMENT, currentToken.value, currentToken.position);
    advance();
    return commentNode;
}

// 继承、删除、插入、使用操作（简化实现）
std::unique_ptr<BaseNode> Parser::parseInherit() {
    auto node = std::make_unique<BaseNode>(NodeType::INHERIT, currentToken.value, currentToken.position);
    advance();
    return node;
}

std::unique_ptr<BaseNode> Parser::parseDelete() {
    auto node = std::make_unique<BaseNode>(NodeType::DELETE, currentToken.value, currentToken.position);
    advance();
    return node;
}

std::unique_ptr<BaseNode> Parser::parseInsert() {
    auto node = std::make_unique<BaseNode>(NodeType::INSERT, currentToken.value, currentToken.position);
    advance();
    return node;
}

std::unique_ptr<BaseNode> Parser::parseUse() {
    auto node = std::make_unique<BaseNode>(NodeType::USE, currentToken.value, currentToken.position);
    advance();
    return node;
}

// 辅助方法
String Parser::parseLiteral() {
    String literal;
    
    switch (currentToken.type) {
        case TokenType::LITERAL_STRING:
        case TokenType::LITERAL_UNQUOTED:
        case TokenType::LITERAL_NUMBER:
            literal = currentToken.value;
            advance();
            break;
        default:
            reportError("期望字面量", "MISSING_LITERAL");
            break;
    }
    
    return literal;
}

// Token操作
Token Parser::nextToken() {
    if (lexer) {
        return lexer->nextToken();
    }
    return Token(TokenType::EOF_TOKEN, "", Position{});
}

Token Parser::peekToken(size_t lookahead) {
    if (lexer) {
        return lexer->peekToken(lookahead);
    }
    return Token(TokenType::EOF_TOKEN, "", Position{});
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) const {
    return currentToken.type == type;
}

Token Parser::consume(TokenType type, const String& errorMessage) {
    if (check(type)) {
        Token token = currentToken;
        advance();
        return token;
    }
    
    reportError(errorMessage.empty() ? "意外的Token" : errorMessage, "UNEXPECTED_TOKEN");
    return Token(TokenType::UNKNOWN, "", currentToken.position);
}

void Parser::advance() {
    if (!isAtEnd()) {
        currentToken = nextToken();
    }
}

// 错误处理
void Parser::reportError(const String& message, const String& errorCode, const String& suggestion) {
    errors.emplace_back(message, currentToken.position, errorCode, suggestion);
}

void Parser::reportUnexpectedToken(TokenType expected) {
    String message = "期望 " + Token::tokenTypeToString(expected) + 
                    " 但得到 " + Token::tokenTypeToString(currentToken.type);
    reportError(message, "UNEXPECTED_TOKEN");
}

bool Parser::recover() {
    // 简化的错误恢复
    advance();
    return !isAtEnd();
}

// 检查方法
bool Parser::isAtEnd() const {
    return currentToken.type == TokenType::EOF_TOKEN;
}

bool Parser::isAttribute() const {
    return currentToken.type == TokenType::IDENTIFIER && 
           (peekToken().type == TokenType::COLON || peekToken().type == TokenType::EQUALS);
}

} // namespace CHTL