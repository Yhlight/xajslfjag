#include "Parser.h"
#include "../CHTLNode/ConstraintNode.h"
#include "../CHTLNode/SpecializationNode.h"
#include "../CHTLConfig/ConfigurationManager.h"
#include "../CHTLLoader/ImportEnhancer.h"
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
    
    // 初始化选择器自动化管理器
    SelectorAutomationConfig autoConfig;
    autoConfig.disableStyleAutoAddClass = !config.enableStyleAutomation;
    autoConfig.disableScriptAutoAddClass = !config.enableScriptAutomation;
    
    selectorAutomation = std::make_shared<SelectorAutomationManager>(autoConfig);
    referenceRuleManager = std::make_shared<ReferenceRuleManager>();
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
    
    // 在解析完成后处理引用规则和选择器自动化
    if (document) {
        processReferenceRules(document.get());
        
        // 递归处理所有元素节点的选择器自动化
        processDocumentSelectorAutomation(document.get());
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
        case TokenType::NAME:
            return parseNameConfiguration();
        case TokenType::ORIGIN_TYPE:
            return parseOriginTypeConfiguration();
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
        case TokenType::EXCEPT:
            return parseExcept();
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

// 导入解析 (增强版)
std::unique_ptr<BaseNode> Parser::parseImport() {
    Position pos = currentToken.position;
    advance(); // 消费 '[Import]'
    
    // 解析导入类型
    String importType;
    bool isOriginImport = false;
    
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
    } else if (check(TokenType::ORIGIN)) {
        // [Import] [Origin] @Vue as CustomVue from "..."
        advance(); // 消费 '[Origin]'
        isOriginImport = true;
        
        // 解析原始类型
        if (currentToken.value.find("@") == 0) {
            importType = currentToken.value.substr(1); // 移除@前缀
            advance();
        } else {
            reportError("期望原始类型 (如@Vue)", "EXPECT_ORIGIN_TYPE");
            return nullptr;
        }
    } else {
        reportError("期望导入类型", "MISSING_IMPORT_TYPE");
        return nullptr;
    }
    
    // 解析可选的 as 语法 (对于原始嵌入)
    String asClause;
    if (isOriginImport && check(TokenType::AS)) {
        advance(); // 消费 'as'
        String aliasName = consume(TokenType::IDENTIFIER, "期望别名").value;
        asClause = importType + " as " + aliasName;
    }
    
    // 解析 from 关键字
    consume(TokenType::FROM, "期望 'from'");
    
    // 解析源路径 (支持通配符)
    String sourcePath = parseLiteral();
    
    // 使用Import增强器处理导入
    auto& importEnhancer = getGlobalImportEnhancer();
    
    // 检查是否为通配符路径
    if (importEnhancer.isWildcardPath(sourcePath)) {
        // 处理通配符导入
        auto wildcardPaths = importEnhancer.processWildcardImports(importType, sourcePath);
        
        if (!wildcardPaths.empty()) {
            // 为第一个匹配的路径创建导入节点
            auto importNode = importEnhancer.processEnhancedImport(importType, wildcardPaths[0], asClause);
            importNode->position = pos;
            
            // 标记为通配符导入
            importNode->setWildcardImport(true);
            
            // 存储所有匹配的路径
            for (size_t i = 1; i < wildcardPaths.size(); ++i) {
                importNode->importMetadata["wildcard_path_" + std::to_string(i)] = wildcardPaths[i];
            }
            
            return std::unique_ptr<BaseNode>(importNode.release());
        } else {
            reportError("未找到匹配通配符模式的文件: " + sourcePath, "WILDCARD_NO_MATCH");
            return nullptr;
        }
    } else {
        // 处理标准导入
        auto importNode = importEnhancer.processEnhancedImport(importType, sourcePath, asClause);
        importNode->position = pos;
        
        // 验证路径
        if (!importEnhancer.validateImportPath(sourcePath)) {
            reportError("无效的导入路径: " + sourcePath, "INVALID_IMPORT_PATH");
        }
        
        // 处理原始嵌入的别名
        if (isOriginImport && !asClause.empty()) {
            auto& namedOriginManager = getGlobalNamedOriginManager();
            auto [originType, aliasName] = importEnhancer.parseAsClause(asClause);
            
            if (!aliasName.empty()) {
                // 注册命名原始嵌入
                NamedOriginConfig config(originType, aliasName);
                namedOriginManager.registerNamedOrigin(aliasName, config);
                
                // 设置ImportNode的原始嵌入信息
                importNode->importMetadata["is_named_origin"] = "true";
                importNode->importMetadata["origin_type"] = originType;
                importNode->importMetadata["alias_name"] = aliasName;
            }
        }
        
        // 解析可选的 as 语法 (对于其他类型的导入)
        if (!isOriginImport && check(TokenType::AS)) {
            advance();
            String alias = consume(TokenType::IDENTIFIER, "期望别名").value;
            importNode->setAlias(alias);
        }
        
        // 解析可选的 except 语法
        if (check(TokenType::EXCEPT)) {
            advance();
            
            // 解析排除项列表
            while (!isAtEnd() && currentToken.type != TokenType::SEMICOLON) {
                if (currentToken.type == TokenType::IDENTIFIER) {
                    importNode->addExceptItem(currentToken.value);
                    advance();
                }
                
                if (check(TokenType::COMMA)) {
                    advance();
                }
            }
        }
        
        return std::unique_ptr<BaseNode>(importNode.release());
    }
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

// 继承解析（完整实现）
std::unique_ptr<BaseNode> Parser::parseInherit() {
    Position startPos = currentToken.position;
    advance(); // 跳过 'inherit'
    
    // 期待 @Type Name 格式
    if (currentToken.type != TokenType::AT) {
        reportError("Expected '@' after 'inherit'");
        return nullptr;
    }
    advance(); // 跳过 '@'
    
    if (currentToken.type != TokenType::IDENTIFIER) {
        reportError("Expected type identifier after '@'");
        return nullptr;
    }
    
    String inheritanceType = currentToken.value;
    advance();
    
    if (currentToken.type != TokenType::IDENTIFIER) {
        reportError("Expected name identifier after type");
        return nullptr;
    }
    
    String inheritanceName = currentToken.value;
    advance();
    
    // 创建继承节点
    auto inheritNode = std::make_unique<BaseNode>(NodeType::INHERIT, 
                                                  "inherit @" + inheritanceType + " " + inheritanceName, 
                                                  startPos);
    
    // 设置继承信息作为属性
    inheritNode->setAttribute("inheritanceType", inheritanceType);
    inheritNode->setAttribute("inheritanceName", inheritanceName);
    inheritNode->setAttribute("isExplicit", "true");
    
    return inheritNode;
}

std::unique_ptr<BaseNode> Parser::parseDelete() {
    // delete line-height, border;
    // delete @Style WhiteText;
    // delete span;
    // delete div[1];
    
    Position startPos = currentToken.position;
    
    if (!check(TokenType::DELETE)) {
        reportError("期望 'delete' 关键字", "EXPECT_DELETE");
        return nullptr;
    }
    consume(TokenType::DELETE);
    
    // 收集删除目标
    StringVector targets;
    String targetsString;
    
    while (!isAtEnd() && currentToken.type != TokenType::SEMICOLON) {
        if (!targetsString.empty()) {
            targetsString += " ";
        }
        targetsString += currentToken.value;
        advance();
        
        // 处理逗号分隔
        if (currentToken.type == TokenType::COMMA) {
            targets.push_back(targetsString);
            targetsString.clear();
            advance(); // 消费逗号
        }
    }
    
    // 添加最后一个目标
    if (!targetsString.empty()) {
        targets.push_back(targetsString);
    }
    
    if (targets.empty()) {
        reportError("delete 后必须指定删除目标", "EXPECT_DELETE_TARGETS");
        return nullptr;
    }
    
    // 创建删除节点
    auto deleteNode = std::make_unique<DeleteNode>(startPos);
    
    // 确定删除类型并设置目标
    if (targets.size() == 1) {
        String target = targets[0];
        
        // 检查是否为继承删除
        if (target.find("@Style") != String::npos || 
            target.find("@Element") != String::npos || 
            target.find("@Var") != String::npos) {
            deleteNode->setInheritanceTarget(target);
        }
        // 检查是否为索引访问元素删除
        else if (IndexAccessNode::isIndexAccessSyntax(target)) {
            auto [elementName, index] = IndexAccessNode::parseIndexAccess(target);
            deleteNode->setElementTarget(elementName, index);
        }
        // 普通元素删除
        else if (HTMLElementMap::isHTMLElement(target)) {
            deleteNode->setElementTarget(target);
        }
        // 默认为属性删除
        else {
            deleteNode->addTarget(target);
        }
    } else {
        // 多个目标，通常是属性删除
        deleteNode->addTargets(targets);
    }
    
    // 消费分号
    consume(TokenType::SEMICOLON);
    
    return std::unique_ptr<BaseNode>(deleteNode.release());
}

std::unique_ptr<BaseNode> Parser::parseInsert() {
    // insert after div[0] { ... }
    // insert before span { ... }
    // insert at top { ... }
    
    Position startPos = currentToken.position;
    
    if (!check(TokenType::INSERT)) {
        reportError("期望 'insert' 关键字", "EXPECT_INSERT");
        return nullptr;
    }
    consume(TokenType::INSERT);
    
    // 解析插入位置
    InsertPosition position = InsertPosition::AFTER; // 默认
    String target;
    
    if (currentToken.value == "after") {
        position = InsertPosition::AFTER;
        advance();
        
        // 获取目标选择器
        if (currentToken.type != TokenType::LBRACE) {
            target = currentToken.value;
            advance();
        }
    }
    else if (currentToken.value == "before") {
        position = InsertPosition::BEFORE;
        advance();
        
        if (currentToken.type != TokenType::LBRACE) {
            target = currentToken.value;
            advance();
        }
    }
    else if (currentToken.value == "replace") {
        position = InsertPosition::REPLACE;
        advance();
        
        if (currentToken.type != TokenType::LBRACE) {
            target = currentToken.value;
            advance();
        }
    }
    else if (currentToken.value == "at") {
        advance();
        if (currentToken.value == "top") {
            position = InsertPosition::AT_TOP;
            advance();
        }
        else if (currentToken.value == "bottom") {
            position = InsertPosition::AT_BOTTOM;
            advance();
        }
        else {
            reportError("期望 'top' 或 'bottom'", "EXPECT_TOP_BOTTOM");
            return nullptr;
        }
    }
    
    // 创建插入节点
    auto insertNode = std::make_unique<InsertNode>(position, startPos);
    if (!target.empty()) {
        insertNode->setTarget(target);
    }
    
    // 解析插入内容块
    if (check(TokenType::LBRACE)) {
        advance(); // 消费 '{'
        
        // 解析块内容
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            if (auto content = parseStatement()) {
                insertNode->addContent(std::move(content));
            }
        }
        
        consume(TokenType::RBRACE, "期望 '}'");
    }
    else {
        reportError("期望插入内容块 '{'", "EXPECT_INSERT_BLOCK");
        return nullptr;
    }
    
    return std::unique_ptr<BaseNode>(insertNode.release());
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

Token Parser::peekToken(size_t lookahead) const {
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

// ========== 约束解析实现 ==========

std::unique_ptr<ConstraintNode> Parser::parseConstraint() {
    if (currentToken.type == TokenType::EXCEPT) {
        return parseExcept();
    }
    
    reportError("期望约束关键字", "EXPECT_CONSTRAINT");
    return nullptr;
}

std::unique_ptr<ConstraintNode> Parser::parseExcept() {
    // except span, [Custom] @Element Box;
    Position startPos = currentToken.position;
    
    if (!check(TokenType::EXCEPT)) {
        reportError("期望 'except' 关键字", "EXPECT_EXCEPT");
        return nullptr;
    }
    consume(TokenType::EXCEPT);
    
    // 解析约束目标列表
    StringVector targets = parseConstraintTargets();
    if (targets.empty()) {
        reportError("except 后必须指定约束目标", "EXPECT_CONSTRAINT_TARGETS");
        return nullptr;
    }
    
    // 确定约束类型
    ConstraintType constraintType = ConstraintType::PRECISE; // 默认精确约束
    
    // 检查是否为类型约束
    for (const auto& target : targets) {
        if (target == "@Html" || target == "[Custom]" || target == "[Template]") {
            constraintType = ConstraintType::TYPE;
            break;
        }
    }
    
    // 检查是否为全局约束 - 通过上下文判断
    String currentScope = getCurrentScope();
    if (isInNamespaceScope()) {
        constraintType = ConstraintType::GLOBAL;
    }
    
    // 创建约束节点
    auto constraint = std::make_unique<ConstraintNode>(constraintType, startPos);
    
    // 解析约束目标
    for (const auto& targetStr : targets) {
        auto target = ConstraintNode::parseConstraintTarget(targetStr);
        constraint->addTarget(target);
    }
    
    // 设置作用域
    if (constraintType == ConstraintType::GLOBAL) {
        constraint->scope = currentScope;
    }
    
    // 消费分号
    consume(TokenType::SEMICOLON);
    
    return constraint;
}

StringVector Parser::parseConstraintTargets() {
    StringVector targets;
    
    while (!isAtEnd() && currentToken.type != TokenType::SEMICOLON) {
        String target;
        
        // 解析一个约束目标
        if (currentToken.type == TokenType::LEFT_BRACKET) {
            // [Custom] @Element Box 或 [Template] @Style 等
            target += currentToken.value;
            advance();
            
            // 解析括号内的内容
            while (!isAtEnd() && currentToken.type != TokenType::RIGHT_BRACKET) {
                target += currentToken.value;
                advance();
            }
            
            if (currentToken.type == TokenType::RIGHT_BRACKET) {
                target += currentToken.value;
                advance();
            }
            
            // 继续解析后续内容 (@Element, @Style 等)
            while (!isAtEnd() && 
                   currentToken.type != TokenType::COMMA && 
                   currentToken.type != TokenType::SEMICOLON) {
                target += " " + currentToken.value;
                advance();
            }
        }
        else if (currentToken.type == TokenType::AT_HTML ||
                 currentToken.type == TokenType::AT_STYLE ||
                 currentToken.type == TokenType::AT_JAVASCRIPT) {
            // @Html, @Style, @JavaScript
            target = currentToken.value;
            advance();
        }
        else if (currentToken.value.find("@") == 0) {
            // 自定义原始嵌入类型 @Vue
            target = currentToken.value;
            advance();
        }
        else {
            // HTML元素名或其他标识符
            target = currentToken.value;
            advance();
        }
        
        if (!target.empty()) {
            targets.push_back(target);
        }
        
        // 处理逗号分隔
        if (currentToken.type == TokenType::COMMA) {
            advance();
        }
    }
    
    return targets;
}

// 辅助方法
String Parser::getCurrentScope() const {
    // 遍历解析栈，找到当前的命名空间
    if (context) {
        return context->getCurrentNamespace();
    }
    
    return ""; // 默认全局作用域
}

bool Parser::isInNamespaceScope() const {
    // 检查当前是否在命名空间内部
    if (context) {
        return !context->getCurrentNamespace().empty();
    }
    
    return false;
}

// ========== 特例化解析实现 ==========

std::unique_ptr<IndexAccessNode> Parser::parseIndexAccess() {
    // div[1] { ... }
    
    String elementName = currentToken.value;
    Position startPos = currentToken.position;
    advance();
    
    if (!check(TokenType::LEFT_BRACKET)) {
        reportError("期望 '['", "EXPECT_LEFT_BRACKET");
        return nullptr;
    }
    consume(TokenType::LEFT_BRACKET);
    
    if (currentToken.type != TokenType::LITERAL_NUMBER) {
        reportError("期望索引数字", "EXPECT_INDEX_NUMBER");
        return nullptr;
    }
    
    size_t index = std::stoul(currentToken.value);
    advance();
    
    if (!check(TokenType::RIGHT_BRACKET)) {
        reportError("期望 ']'", "EXPECT_RIGHT_BRACKET");
        return nullptr;
    }
    consume(TokenType::RIGHT_BRACKET);
    
    auto indexNode = std::make_unique<IndexAccessNode>(elementName, index, startPos);
    
    // 解析内容块 (可选)
    if (check(TokenType::LBRACE)) {
        advance(); // 消费 '{'
        
        // 解析块内容
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            if (auto content = parseStatement()) {
                indexNode->setContent(std::move(content));
                break; // 索引访问通常只包含一个内容节点
            }
        }
        
        consume(TokenType::RBRACE, "期望 '}'");
    }
    
    return indexNode;
}

std::unique_ptr<NoValueStyleNode> Parser::parseNoValueStyle() {
    // color, font-size;
    
    Position startPos = currentToken.position;
    StringVector properties;
    
    // 收集属性名称
    while (!isAtEnd() && currentToken.type != TokenType::SEMICOLON) {
        if (currentToken.type == TokenType::IDENTIFIER) {
            properties.push_back(currentToken.value);
            advance();
        }
        
        // 处理逗号分隔
        if (currentToken.type == TokenType::COMMA) {
            advance();
        } else if (currentToken.type != TokenType::SEMICOLON) {
            break; // 遇到非逗号非分号，停止解析
        }
    }
    
    if (properties.empty()) {
        reportError("无值样式组必须包含至少一个属性", "EXPECT_PROPERTIES");
        return nullptr;
    }
    
    // 消费分号
    consume(TokenType::SEMICOLON);
    
    auto noValueNode = std::make_unique<NoValueStyleNode>(startPos);
    noValueNode->addProperties(properties);
    
    return noValueNode;
}

bool Parser::isIndexAccessSyntax(const String& input) const {
    return IndexAccessNode::isIndexAccessSyntax(input);
}

bool Parser::isNoValueStyleSyntax(const String& input) const {
    return NoValueStyleNode::isNoValueStyleSyntax(input);
}

// ========== 配置解析实现 ==========

std::unique_ptr<BaseNode> Parser::parseNameConfiguration() {
    // [Name] { CUSTOM_STYLE = [@Style, @style, @CSS]; ... }
    
    Position startPos = currentToken.position;
    
    if (!check(TokenType::NAME)) {
        reportError("期望 '[Name]' 关键字", "EXPECT_NAME");
        return nullptr;
    }
    consume(TokenType::NAME);
    
    auto nameConfigNode = std::make_unique<BaseNode>(NodeType::CONFIG_NAME, "Name", startPos);
    
    if (!check(TokenType::LBRACE)) {
        reportError("期望 '{'", "EXPECT_LBRACE");
        return nullptr;
    }
    consume(TokenType::LBRACE);
    
    // 解析名称配置项
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (currentToken.type == TokenType::IDENTIFIER) {
            String configKey = currentToken.value;
            advance();
            
            if (!check(TokenType::EQUALS)) {
                reportError("期望 '='", "EXPECT_EQUALS");
                continue;
            }
            consume(TokenType::EQUALS);
            
            // 解析配置值
            String configValue;
            
            // 处理组选项语法 [option1, option2, ...]
            if (check(TokenType::LEFT_BRACKET)) {
                configValue += currentToken.value;
                advance();
                
                while (!check(TokenType::RIGHT_BRACKET) && !isAtEnd()) {
                    configValue += currentToken.value;
                    advance();
                }
                
                if (check(TokenType::RIGHT_BRACKET)) {
                    configValue += currentToken.value;
                    advance();
                }
            } else {
                // 单一配置值
                configValue = currentToken.value;
                advance();
            }
            
            // 添加为属性
            nameConfigNode->setAttribute(configKey, configValue);
            
            // 消费分号
            consume(TokenType::SEMICOLON);
        } else {
            advance(); // 跳过无法识别的token
        }
    }
    
    consume(TokenType::RBRACE, "期望 '}'");
    
    return nameConfigNode;
}

std::unique_ptr<BaseNode> Parser::parseOriginTypeConfiguration() {
    // [OriginType] { ORIGINTYPE_VUE = @Vue; ... }
    
    Position startPos = currentToken.position;
    
    if (!check(TokenType::ORIGIN_TYPE)) {
        reportError("期望 '[OriginType]' 关键字", "EXPECT_ORIGIN_TYPE");
        return nullptr;
    }
    consume(TokenType::ORIGIN_TYPE);
    
    auto originTypeConfigNode = std::make_unique<BaseNode>(NodeType::CONFIG_ORIGIN_TYPE, "OriginType", startPos);
    
    if (!check(TokenType::LBRACE)) {
        reportError("期望 '{'", "EXPECT_LBRACE");
        return nullptr;
    }
    consume(TokenType::LBRACE);
    
    // 解析原始类型配置项
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (currentToken.type == TokenType::IDENTIFIER) {
            String configKey = currentToken.value;
            advance();
            
            if (!check(TokenType::EQUALS)) {
                reportError("期望 '='", "EXPECT_EQUALS");
                continue;
            }
            consume(TokenType::EQUALS);
            
            // 解析配置值 (应该是@开头的类型名)
            String configValue = currentToken.value;
            advance();
            
            // 验证原始类型格式
            if (configKey.find("ORIGINTYPE_") != 0) {
                reportError("原始类型配置键必须以 'ORIGINTYPE_' 开头", "INVALID_ORIGIN_TYPE_KEY");
                continue;
            }
            
            if (configValue.empty() || configValue[0] != '@') {
                reportError("原始类型值必须以 '@' 开头", "INVALID_ORIGIN_TYPE_VALUE");
                continue;
            }
            
            // 添加为属性
            originTypeConfigNode->setAttribute(configKey, configValue);
            
            // 消费分号
            consume(TokenType::SEMICOLON);
        } else {
            advance(); // 跳过无法识别的token
        }
    }
    
    consume(TokenType::RBRACE, "期望 '}'");
    
    return originTypeConfigNode;
}

// 选择器自动化处理实现
void Parser::processSelectorAutomation(BaseNode* elementNode) {
    if (!elementNode || !selectorAutomation) return;
    
    // 自动添加选择器到元素
    selectorAutomation->autoAddSelectorsToElement(elementNode);
    
    // 处理子节点中的样式和脚本块
    for (auto& child : elementNode->getChildren()) {
        if (child->getType() == NodeType::STYLE) {
            processStyleBlockSelectors(elementNode, child.get());
        } else if (child->getType() == NodeType::SCRIPT) {
            processScriptBlockSelectors(elementNode, child.get());
        }
    }
}

void Parser::processStyleBlockSelectors(BaseNode* elementNode, BaseNode* styleNode) {
    if (!selectorAutomation) return;
    
    selectorAutomation->processLocalStyleSelectors(elementNode, styleNode);
}

void Parser::processScriptBlockSelectors(BaseNode* elementNode, BaseNode* scriptNode) {
    if (!selectorAutomation) return;
    
    selectorAutomation->processLocalScriptSelectors(elementNode, scriptNode);
}

void Parser::processReferenceRules(BaseNode* documentNode) {
    if (!referenceRuleManager || !selectorAutomation) return;
    
    referenceRuleManager->processReferences(documentNode, *selectorAutomation);
}

void Parser::processDocumentSelectorAutomation(BaseNode* documentNode) {
    if (!documentNode) return;
    
    // 递归处理所有元素节点
    if (documentNode->getType() == NodeType::ELEMENT) {
        processSelectorAutomation(documentNode);
    }
    
    // 递归处理子节点
    for (auto& child : documentNode->getChildren()) {
        processDocumentSelectorAutomation(child.get());
    }
}

} // namespace CHTL