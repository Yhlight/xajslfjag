#include "CHTLParser.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<CHTLToken>& tokenList) 
    : tokens(tokenList), current(0) {
    stateStack.push(CHTLParsingState::GlobalScope);
}

// === 基础Token操作 ===

CHTLToken CHTLParser::peek(size_t offset) const {
    size_t index = current + offset;
    if (index >= tokens.size()) {
        return CHTLToken(CHTLTokenType::EOF_TOKEN);
    }
    return tokens[index];
}

CHTLToken CHTLParser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

bool CHTLParser::isAtEnd() const {
    return current >= tokens.size() || peek().type == CHTLTokenType::EOF_TOKEN;
}

bool CHTLParser::match(CHTLTokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool CHTLParser::check(CHTLTokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

CHTLToken CHTLParser::consume(CHTLTokenType type, const std::string& message) {
    if (check(type)) return advance();
    
    addError("期望 " + message + "，但获得 " + peek().getTypeName());
    return peek();
}

// === 状态管理 ===

void CHTLParser::pushState(CHTLParsingState state) {
    stateStack.push(state);
}

void CHTLParser::popState() {
    if (!stateStack.empty()) {
        stateStack.pop();
    }
}

CHTLParsingState CHTLParser::getCurrentState() const {
    return stateStack.empty() ? CHTLParsingState::GlobalScope : stateStack.top();
}

// === 错误处理 ===

void CHTLParser::addError(const std::string& message) {
    CHTLToken token = peek();
    std::stringstream ss;
    ss << "行 " << token.line << "，列 " << token.column << ": " << message;
    errors.push_back(ss.str());
}

// === 主解析函数 ===

std::shared_ptr<CHTLDocumentNode> CHTLParser::parseDocument() {
    auto document = std::make_shared<CHTLDocumentNode>();
    
    skipWhitespaceAndComments();
    
    while (!isAtEnd()) {
        size_t oldCurrent = current;
        auto declaration = parseDeclaration();
        if (declaration) {
            document->addChild(declaration);
        }
        skipWhitespaceAndComments();
        
        // 防止无限循环：如果没有前进，强制前进
        if (current == oldCurrent && !isAtEnd()) {
            addError("解析停滞，跳过Token: " + peek().value);
            advance();
        }
    }
    
    return document;
}

// === CHTL语法解析 - 严格按照语法文档 ===

std::shared_ptr<CHTLNode> CHTLParser::parseDeclaration() {
    skipWhitespaceAndComments();
    
    if (isAtEnd()) return nullptr;
    
    // use声明
    if (check(CHTLTokenType::USE)) {
        return parseUseDeclaration();
    }
    
    // 方括号关键字声明
    if (check(CHTLTokenType::LBRACKET_NAMESPACE)) {
        return parseNamespaceDeclaration();
    }
    
    if (check(CHTLTokenType::LBRACKET_TEMPLATE)) {
        return parseTemplateDefinition();
    }
    
    if (check(CHTLTokenType::LBRACKET_CUSTOM)) {
        return parseCustomDefinition();
    }
    
    if (check(CHTLTokenType::LBRACKET_ORIGIN)) {
        return parseOriginDefinition();
    }
    
    if (check(CHTLTokenType::LBRACKET_CONFIGURATION)) {
        return parseConfigurationDefinition();
    }
    
    if (check(CHTLTokenType::LBRACKET_IMPORT)) {
        return parseImportStatement();
    }
    
    // 类型标识符使用（模板/自定义使用）
    if (check(CHTLTokenType::AT_STYLE) || check(CHTLTokenType::AT_ELEMENT) || check(CHTLTokenType::AT_VAR)) {
        return parseTemplateUsage();
    }
    
    // 选择器自动化
    if (check(CHTLTokenType::CLASS_SELECTOR) || check(CHTLTokenType::ID_SELECTOR)) {
        return parseAutoSelector();
    }
    
    // HTML元素或text节点
    if (check(CHTLTokenType::TEXT)) {
        return parseTextNode();
    }
    
    if (check(CHTLTokenType::IDENTIFIER)) {
        return parseElement();
    }
    
    // 跳过无法识别的Token
    addError("无法识别的声明: " + peek().value);
    advance();
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseUseDeclaration() {
    consume(CHTLTokenType::USE, "use");
    
    auto useNode = CHTLNodeFactory::createUseDeclaration("");
    
    if (check(CHTLTokenType::IDENTIFIER)) {
        // use html5;
        CHTLToken type = advance();
        useNode->value = type.value;
    } else if (check(CHTLTokenType::AT_CONFIG)) {
        // use @Config Basic;
        advance(); // @Config
        if (check(CHTLTokenType::IDENTIFIER)) {
            CHTLToken name = advance();
            useNode->value = "@Config " + name.value;
        }
    }
    
    match(CHTLTokenType::SEMICOLON);
    return useNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseNamespaceDeclaration() {
    consume(CHTLTokenType::LBRACKET_NAMESPACE, "[Namespace]");
    
    if (!check(CHTLTokenType::IDENTIFIER)) {
        addError("期望命名空间名称");
        return nullptr;
    }
    
    CHTLToken nameToken = advance();
    auto namespaceNode = CHTLNodeFactory::createNamespaceDeclaration(nameToken.value);
    
    pushState(CHTLParsingState::InNamespace);
    currentNamespace = nameToken.value;
    
    if (match(CHTLTokenType::LBRACE)) {
        // 命名空间内容
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            auto declaration = parseDeclaration();
            if (declaration) {
                namespaceNode->addChild(declaration);
            }
        }
        consume(CHTLTokenType::RBRACE, "}");
    }
    
    popState();
    currentNamespace.clear();
    
    return namespaceNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseTemplateDefinition() {
    consume(CHTLTokenType::LBRACKET_TEMPLATE, "[Template]");
    
    // 类型标识符
    if (!check(CHTLTokenType::AT_STYLE) && !check(CHTLTokenType::AT_ELEMENT) && !check(CHTLTokenType::AT_VAR)) {
        addError("期望模板类型标识符");
        return nullptr;
    }
    
    CHTLToken typeToken = advance();
    std::string templateType = typeToken.value.substr(1); // 去掉@
    
    if (!check(CHTLTokenType::IDENTIFIER)) {
        addError("期望模板名称");
        return nullptr;
    }
    
    CHTLToken nameToken = advance();
    auto templateNode = CHTLNodeFactory::createTemplate(nameToken.value, templateType);
    
    pushState(CHTLParsingState::InTemplateDefinition);
    
    if (match(CHTLTokenType::LBRACE)) {
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            if (templateType == "Style") {
                auto property = parseStyleProperty();
                if (property) templateNode->addChild(property);
            } else if (templateType == "Element") {
                auto element = parseDeclaration();
                if (element) templateNode->addChild(element);
            } else if (templateType == "Var") {
                auto property = parseStyleProperty(); // 变量使用相同格式
                if (property) templateNode->addChild(property);
            }
        }
        consume(CHTLTokenType::RBRACE, "}");
    }
    
    popState();
    return templateNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseCustomDefinition() {
    consume(CHTLTokenType::LBRACKET_CUSTOM, "[Custom]");
    
    // 类型标识符
    if (!check(CHTLTokenType::AT_STYLE) && !check(CHTLTokenType::AT_ELEMENT) && !check(CHTLTokenType::AT_VAR)) {
        addError("期望自定义类型标识符");
        return nullptr;
    }
    
    CHTLToken typeToken = advance();
    std::string customType = typeToken.value.substr(1); // 去掉@
    
    if (!check(CHTLTokenType::IDENTIFIER)) {
        addError("期望自定义名称");
        return nullptr;
    }
    
    CHTLToken nameToken = advance();
    auto customNode = CHTLNodeFactory::createCustom(nameToken.value, customType);
    
    pushState(CHTLParsingState::InCustomDefinition);
    
    if (match(CHTLTokenType::LBRACE)) {
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            if (customType == "Style") {
                auto property = parseStyleProperty();
                if (property) customNode->addChild(property);
            } else if (customType == "Element") {
                auto element = parseDeclaration();
                if (element) customNode->addChild(element);
            } else if (customType == "Var") {
                auto property = parseStyleProperty();
                if (property) customNode->addChild(property);
            }
        }
        consume(CHTLTokenType::RBRACE, "}");
    }
    
    popState();
    return customNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseOriginDefinition() {
    // 原始嵌入已经在Lexer中作为ORIGIN_CONTENT Token处理
    if (check(CHTLTokenType::ORIGIN_CONTENT)) {
        CHTLToken originToken = advance();
        auto originNode = CHTLNodeFactory::createNode(CHTLNodeType::ORIGIN_DEFINITION, "origin", originToken.value);
        return originNode;
    }
    
    addError("期望原始嵌入内容");
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseTemplateUsage() {
    CHTLToken typeToken = advance(); // @Style, @Element, @Var
    
    if (!check(CHTLTokenType::IDENTIFIER)) {
        addError("期望模板名称");
        return nullptr;
    }
    
    CHTLToken nameToken = advance();
    auto usageNode = CHTLNodeFactory::createNode(CHTLNodeType::TEMPLATE_USAGE, nameToken.value, typeToken.value);
    
    // 检查是否有特例化块
    if (match(CHTLTokenType::LBRACE)) {
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            auto content = parseDeclaration();
            if (content) usageNode->addChild(content);
        }
        consume(CHTLTokenType::RBRACE, "}");
    } else {
        match(CHTLTokenType::SEMICOLON);
    }
    
    return usageNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseElement() {
    if (!check(CHTLTokenType::IDENTIFIER)) {
        addError("期望元素名称");
        return nullptr;
    }
    
    CHTLToken elementToken = advance();
    auto elementNode = CHTLNodeFactory::createElement(elementToken.value);
    
    pushState(CHTLParsingState::InElement);
    
    if (match(CHTLTokenType::LBRACE)) {
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            skipWhitespaceAndComments();
            
            // 属性解析
            if (check(CHTLTokenType::IDENTIFIER) && (peek(1).type == CHTLTokenType::COLON || peek(1).type == CHTLTokenType::EQUAL)) {
                auto attribute = parseAttribute();
                if (attribute) {
                    elementNode->setAttribute(attribute->name, attribute->value);
                }
                continue;
            }
            
            // 局部样式块
            if (check(CHTLTokenType::STYLE)) {
                auto styleBlock = parseStyleBlock();
                if (styleBlock) elementNode->addChild(styleBlock);
                continue;
            }
            
            // 局部脚本块
            if (check(CHTLTokenType::SCRIPT)) {
                auto scriptBlock = parseScriptBlock();
                if (scriptBlock) elementNode->addChild(scriptBlock);
                continue;
            }
            
            // 子元素或文本节点
            auto child = parseDeclaration();
            if (child) {
                elementNode->addChild(child);
            } else {
                // 如果无法解析，跳过当前Token避免无限循环
                if (!isAtEnd()) {
                    addError("无法解析的内容: " + peek().value);
                    advance();
                }
            }
        }
        consume(CHTLTokenType::RBRACE, "}");
    }
    
    popState();
    return elementNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseTextNode() {
    consume(CHTLTokenType::TEXT, "text");
    
    if (!match(CHTLTokenType::LBRACE)) {
        addError("期望 '{'");
        return nullptr;
    }
    
    std::string textContent;
    
    // 收集文本内容（可以是字面量或标识符）
    while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
        skipWhitespaceAndComments();
        
        if (check(CHTLTokenType::STRING_LITERAL) || 
            check(CHTLTokenType::UNQUOTED_LITERAL) || 
            check(CHTLTokenType::IDENTIFIER) ||
            check(CHTLTokenType::NUMBER)) {
            if (!textContent.empty()) textContent += " ";
            textContent += advance().value;
        } else {
            advance(); // 跳过其他Token
        }
    }
    
    consume(CHTLTokenType::RBRACE, "}");
    
    return CHTLNodeFactory::createTextNode(textContent);
}

std::shared_ptr<CHTLNode> CHTLParser::parseAttribute() {
    if (!check(CHTLTokenType::IDENTIFIER)) {
        addError("期望属性名称");
        return nullptr;
    }
    
    CHTLToken nameToken = advance();
    
    // CE对等式：: 和 = 等价
    if (!match(CHTLTokenType::COLON) && !match(CHTLTokenType::EQUAL)) {
        addError("期望 ':' 或 '='");
        return nullptr;
    }
    
    std::string value;
    if (check(CHTLTokenType::STRING_LITERAL) || 
        check(CHTLTokenType::UNQUOTED_LITERAL) || 
        check(CHTLTokenType::IDENTIFIER) ||
        check(CHTLTokenType::NUMBER)) {
        value = advance().value;
    }
    
    match(CHTLTokenType::SEMICOLON);
    
    auto attributeNode = CHTLNodeFactory::createNode(CHTLNodeType::ATTRIBUTE, nameToken.value, value);
    return attributeNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseStyleBlock() {
    consume(CHTLTokenType::STYLE, "style");
    
    auto styleNode = CHTLNodeFactory::createStyle(true);
    
    pushState(CHTLParsingState::InStyleBlock);
    
    if (match(CHTLTokenType::LBRACE)) {
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            skipWhitespaceAndComments();
            
            auto rule = parseStyleRule();
            if (rule) styleNode->addChild(rule);
        }
        consume(CHTLTokenType::RBRACE, "}");
    }
    
    popState();
    return styleNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseStyleRule() {
    // 选择器规则 (.class { } 或 #id { })
    if (check(CHTLTokenType::CLASS_SELECTOR) || check(CHTLTokenType::ID_SELECTOR) || check(CHTLTokenType::AMPERSAND)) {
        CHTLToken selectorToken = advance();
        
        // 可能有伪类 :hover 等
        std::string selectorValue = selectorToken.value;
        if (match(CHTLTokenType::COLON)) {
            if (check(CHTLTokenType::IDENTIFIER)) {
                selectorValue += ":" + advance().value;
            }
        }
        
        auto ruleNode = CHTLNodeFactory::createNode(CHTLNodeType::STYLE_RULE, selectorValue, "");
        
        if (match(CHTLTokenType::LBRACE)) {
            while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
                auto property = parseStyleProperty();
                if (property) ruleNode->addChild(property);
            }
            consume(CHTLTokenType::RBRACE, "}");
        }
        
        return ruleNode;
    }
    
    // 直接样式属性
    return parseStyleProperty();
}

std::shared_ptr<CHTLNode> CHTLParser::parseStyleProperty() {
    if (!check(CHTLTokenType::IDENTIFIER) && !check(CHTLTokenType::AT_STYLE)) {
        addError("期望属性名称或样式引用");
        return nullptr;
    }
    
    CHTLToken nameToken = advance();
    
    // 样式引用 @Style ButtonStyle;
    if (nameToken.type == CHTLTokenType::AT_STYLE) {
        if (check(CHTLTokenType::IDENTIFIER)) {
            CHTLToken styleNameToken = advance();
            match(CHTLTokenType::SEMICOLON);
            
            auto usageNode = CHTLNodeFactory::createNode(CHTLNodeType::TEMPLATE_USAGE, styleNameToken.value, "@Style");
            return usageNode;
        }
    }
    
    // CE对等式：: 和 = 等价
    if (!match(CHTLTokenType::COLON) && !match(CHTLTokenType::EQUAL)) {
        addError("期望 ':' 或 '='");
        return nullptr;
    }
    
    std::string value;
    
    // 收集属性值（可能有多个Token）
    while (!check(CHTLTokenType::SEMICOLON) && !check(CHTLTokenType::RBRACE) && !isAtEnd()) {
        if (!value.empty()) value += " ";
        
        if (check(CHTLTokenType::IDENTIFIER) && peek(1).type == CHTLTokenType::LPAREN) {
            // 变量调用 ThemeColor(primary)
            CHTLToken varName = advance();
            advance(); // (
            value += varName.value + "(";
            
            while (!check(CHTLTokenType::RPAREN) && !isAtEnd()) {
                value += advance().value;
            }
            
            if (match(CHTLTokenType::RPAREN)) {
                value += ")";
            }
        } else {
            value += advance().value;
        }
    }
    
    match(CHTLTokenType::SEMICOLON);
    
    auto propertyNode = CHTLNodeFactory::createNode(CHTLNodeType::STYLE_PROPERTY, nameToken.value, value);
    return propertyNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseScriptBlock() {
    consume(CHTLTokenType::SCRIPT, "script");
    
    auto scriptNode = CHTLNodeFactory::createScript(true);
    
    pushState(CHTLParsingState::InScriptBlock);
    
    if (match(CHTLTokenType::LBRACE)) {
        std::string scriptContent;
        
        // 收集脚本内容
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            scriptContent += advance().value + " ";
        }
        
        consume(CHTLTokenType::RBRACE, "}");
        
        scriptNode->setAttribute("content", scriptContent);
    }
    
    popState();
    return scriptNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseAutoSelector() {
    CHTLToken selectorToken = advance(); // .class 或 #id
    
    auto elementNode = CHTLNodeFactory::createElement("div"); // 默认为div
    
    // 自动添加class或id属性
    if (selectorToken.type == CHTLTokenType::CLASS_SELECTOR) {
        std::string className = selectorToken.value.substr(1); // 去掉.
        elementNode->setClass(className);
    } else if (selectorToken.type == CHTLTokenType::ID_SELECTOR) {
        std::string idName = selectorToken.value.substr(1); // 去掉#
        elementNode->setId(idName);
    }
    
    pushState(CHTLParsingState::InElement);
    
    if (match(CHTLTokenType::LBRACE)) {
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            auto child = parseDeclaration();
            if (child) elementNode->addChild(child);
        }
        consume(CHTLTokenType::RBRACE, "}");
    }
    
    popState();
    return elementNode;
}

// === 其他解析方法的基本实现 ===

std::shared_ptr<CHTLNode> CHTLParser::parseConfigurationDefinition() {
    // 简化实现，跳过配置内容
    advance(); // [Configuration]
    if (match(CHTLTokenType::LBRACE)) {
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            advance();
        }
        consume(CHTLTokenType::RBRACE, "}");
    }
    return CHTLNodeFactory::createConfigurationDefinition();
}

std::shared_ptr<CHTLNode> CHTLParser::parseImportStatement() {
    // 简化实现
    advance(); // [Import]
    while (!check(CHTLTokenType::SEMICOLON) && !isAtEnd()) {
        advance();
    }
    match(CHTLTokenType::SEMICOLON);
    return CHTLNodeFactory::createNode(CHTLNodeType::IMPORT_STATEMENT, "import", "");
}

std::shared_ptr<CHTLNode> CHTLParser::parseCustomUsage() {
    return parseTemplateUsage(); // 相同逻辑
}

std::shared_ptr<CHTLNode> CHTLParser::parseOriginUsage() {
    return nullptr; // 原始嵌入使用在Origin定义中处理
}

std::shared_ptr<CHTLNode> CHTLParser::parseInheritStatement() {
    consume(CHTLTokenType::INHERIT, "inherit");
    
    auto inheritNode = CHTLNodeFactory::createNode(CHTLNodeType::INHERIT_STATEMENT, "inherit", "");
    
    // 继承目标
    if (check(CHTLTokenType::AT_STYLE) || check(CHTLTokenType::AT_ELEMENT) || check(CHTLTokenType::AT_VAR)) {
        CHTLToken typeToken = advance();
        if (check(CHTLTokenType::IDENTIFIER)) {
            CHTLToken nameToken = advance();
            inheritNode->value = typeToken.value + " " + nameToken.value;
        }
    }
    
    match(CHTLTokenType::SEMICOLON);
    return inheritNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseDeleteStatement() {
    consume(CHTLTokenType::DELETE, "delete");
    
    auto deleteNode = CHTLNodeFactory::createNode(CHTLNodeType::DELETE_STATEMENT, "delete", "");
    
    // 删除目标列表
    std::string targets;
    while (!check(CHTLTokenType::SEMICOLON) && !isAtEnd()) {
        if (!targets.empty()) targets += " ";
        targets += advance().value;
    }
    
    deleteNode->value = targets;
    match(CHTLTokenType::SEMICOLON);
    return deleteNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseInsertStatement() {
    consume(CHTLTokenType::INSERT, "insert");
    
    auto insertNode = CHTLNodeFactory::createNode(CHTLNodeType::INSERT_STATEMENT, "insert", "");
    
    // 位置关键字
    if (check(CHTLTokenType::AFTER) || check(CHTLTokenType::BEFORE) || check(CHTLTokenType::REPLACE) ||
        check(CHTLTokenType::AT_TOP) || check(CHTLTokenType::AT_BOTTOM)) {
        CHTLToken positionToken = advance();
        insertNode->setAttribute("position", positionToken.value);
    }
    
    // 插入内容
    if (match(CHTLTokenType::LBRACE)) {
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            auto content = parseDeclaration();
            if (content) insertNode->addChild(content);
        }
        consume(CHTLTokenType::RBRACE, "}");
    }
    
    return insertNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseExceptClause() {
    consume(CHTLTokenType::EXCEPT, "except");
    
    auto exceptNode = CHTLNodeFactory::createNode(CHTLNodeType::CONSTRAINT_CLAUSE, "except", "");
    
    // 约束目标列表
    std::string targets;
    while (!check(CHTLTokenType::SEMICOLON) && !isAtEnd()) {
        if (!targets.empty()) targets += " ";
        targets += advance().value;
    }
    
    exceptNode->value = targets;
    match(CHTLTokenType::SEMICOLON);
    return exceptNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseVariableUsage() {
    if (!check(CHTLTokenType::IDENTIFIER)) return nullptr;
    
    CHTLToken varName = advance();
    
    if (match(CHTLTokenType::LPAREN)) {
        std::string params;
        while (!check(CHTLTokenType::RPAREN) && !isAtEnd()) {
            if (!params.empty()) params += " ";
            params += advance().value;
        }
        consume(CHTLTokenType::RPAREN, ")");
        
        auto varNode = CHTLNodeFactory::createNode(CHTLNodeType::VARIABLE_USAGE, varName.value, params);
        return varNode;
    }
    
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseConfigurationBlock() {
    // 简化实现
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseLiteral() {
    if (check(CHTLTokenType::STRING_LITERAL)) {
        CHTLToken token = advance();
        return CHTLNodeFactory::createNode(CHTLNodeType::STRING_LITERAL, "string", token.value);
    }
    
    if (check(CHTLTokenType::NUMBER)) {
        CHTLToken token = advance();
        return CHTLNodeFactory::createNode(CHTLNodeType::NUMBER_LITERAL, "number", token.value);
    }
    
    if (check(CHTLTokenType::UNQUOTED_LITERAL)) {
        CHTLToken token = advance();
        return CHTLNodeFactory::createNode(CHTLNodeType::UNQUOTED_LITERAL, "unquoted", token.value);
    }
    
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseIdentifier() {
    if (!check(CHTLTokenType::IDENTIFIER)) return nullptr;
    
    CHTLToken token = advance();
    return CHTLNodeFactory::createNode(CHTLNodeType::IDENTIFIER, "identifier", token.value);
}

// === 辅助方法 ===

bool CHTLParser::isDeclaration() const {
    return check(CHTLTokenType::USE) || 
           check(CHTLTokenType::LBRACKET_NAMESPACE) ||
           check(CHTLTokenType::LBRACKET_TEMPLATE) ||
           check(CHTLTokenType::LBRACKET_CUSTOM) ||
           check(CHTLTokenType::LBRACKET_ORIGIN) ||
           check(CHTLTokenType::LBRACKET_CONFIGURATION) ||
           check(CHTLTokenType::LBRACKET_IMPORT);
}

bool CHTLParser::isElement() const {
    return check(CHTLTokenType::IDENTIFIER) && isValidHTMLElement(peek().value);
}

bool CHTLParser::isTextNode() const {
    return check(CHTLTokenType::TEXT);
}

bool CHTLParser::isStyleBlock() const {
    return check(CHTLTokenType::STYLE);
}

bool CHTLParser::isScriptBlock() const {
    return check(CHTLTokenType::SCRIPT);
}

bool CHTLParser::isAutoSelector() const {
    return check(CHTLTokenType::CLASS_SELECTOR) || check(CHTLTokenType::ID_SELECTOR);
}

bool CHTLParser::isValidHTMLElement(const std::string& name) const {
    // 简化验证：所有标识符都可以是HTML元素
    return !name.empty();
}

bool CHTLParser::isValidTemplateType(const std::string& type) const {
    return type == "Style" || type == "Element" || type == "Var";
}

bool CHTLParser::isValidOriginType(const std::string& type) const {
    return type == "Html" || type == "Style" || type == "JavaScript";
}

void CHTLParser::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        if (check(CHTLTokenType::WHITESPACE) || 
            check(CHTLTokenType::NEWLINE) ||
            check(CHTLTokenType::LINE_COMMENT) ||
            check(CHTLTokenType::BLOCK_COMMENT) ||
            check(CHTLTokenType::GENERATOR_COMMENT)) {
            advance();
        } else {
            break;
        }
    }
}

bool CHTLParser::checkMultiWordKeyword(const std::string& first, const std::string& second) const {
    return (first == "at" && (second == "top" || second == "bottom"));
}

} // namespace CHTL