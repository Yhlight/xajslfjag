#include "CHTLParser.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

// 构造函数
CHTLParser::CHTLParser(const std::vector<CHTLToken>& tokenList) 
    : tokens(tokenList), current(0) {
    stateStack.push(CHTLParsingState::GlobalScope);
}

// === Token操作 ===

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

bool CHTLParser::isInState(CHTLParsingState state) const {
    return getCurrentState() == state;
}

// === 错误处理 ===

void CHTLParser::addError(const std::string& message) {
    CHTLToken token = peek();
    std::stringstream ss;
    ss << "行 " << token.line << "，列 " << token.column << ": " << message;
    errors.push_back(ss.str());
}

void CHTLParser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (peek().type == CHTLTokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case CHTLTokenType::TEMPLATE:
            case CHTLTokenType::CUSTOM:
            case CHTLTokenType::ORIGIN:
            case CHTLTokenType::IMPORT:
            case CHTLTokenType::NAMESPACE:
            case CHTLTokenType::CONFIGURATION:
            case CHTLTokenType::USE:
                return;
            default:
                break;
        }
        
        advance();
    }
}

// === 主解析函数 ===

std::shared_ptr<CHTLDocumentNode> CHTLParser::parseDocument() {
    auto document = CHTLNodeFactory::createDocument();
    
    // 跳过前导空白和注释
    while (match(CHTLTokenType::WHITESPACE) || match(CHTLTokenType::NEWLINE) || isComment()) {
        if (isComment()) {
            document->addChild(parseComment());
        }
    }
    
    // 解析文档内容
    while (!isAtEnd()) {
        try {
            if (match(CHTLTokenType::WHITESPACE) || match(CHTLTokenType::NEWLINE)) {
                continue;
            }
            
            auto node = parseDeclaration();
            if (node) {
                document->addChild(node);
            }
        } catch (const std::exception& e) {
            addError("解析错误: " + std::string(e.what()));
            synchronize();
        }
    }
    
    return document;
}

// === 顶级声明解析 ===

std::shared_ptr<CHTLNode> CHTLParser::parseDeclaration() {
    // 注释
    if (isComment()) {
        return parseComment();
    }
    
    // use声明
    if (check(CHTLTokenType::USE)) {
        return parseUseDeclaration();
    }
    
    // namespace声明
    if (check(CHTLTokenType::NAMESPACE)) {
        return parseNamespaceDeclaration();
    }
    
    // import语句
    if (check(CHTLTokenType::IMPORT)) {
        return parseImportStatement();
    }
    
    // configuration定义
    if (check(CHTLTokenType::CONFIGURATION)) {
        return parseConfigurationDefinition();
    }
    
    // template定义
    if (check(CHTLTokenType::TEMPLATE)) {
        return parseTemplateDefinition();
    }
    
    // custom定义
    if (check(CHTLTokenType::CUSTOM)) {
        return parseCustomDefinition();
    }
    
    // origin定义
    if (check(CHTLTokenType::ORIGIN)) {
        return parseOriginDefinition();
    }
    
    // HTML元素或模板使用
    if (isElement() || isTemplateUsage() || isCustomUsage()) {
        return parseElement();
    }
    
    // 文本节点
    if (check(CHTLTokenType::STRING_LITERAL) || check(CHTLTokenType::UNQUOTED_LITERAL)) {
        return parseTextNode();
    }
    
    addError("未知的声明类型");
    advance();
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseUseDeclaration() {
    consume(CHTLTokenType::USE, "use");
    
    // 解析use类型
    std::string useType;
    if (check(CHTLTokenType::AT_STYLE)) {
        useType = "@style";
        advance();
    } else if (check(CHTLTokenType::AT_ELEMENT)) {
        useType = "@element";
        advance();
    } else if (check(CHTLTokenType::AT_VAR)) {
        useType = "@var";
        advance();
    } else if (check(CHTLTokenType::AT_HTML)) {
        useType = "@html";
        advance();
    } else if (check(CHTLTokenType::AT_JAVASCRIPT)) {
        useType = "@javascript";
        advance();
    } else if (check(CHTLTokenType::AT_CHTL)) {
        useType = "@chtl";
        advance();
    } else if (check(CHTLTokenType::AT_CJMOD)) {
        useType = "@cjmod";
        advance();
    } else if (check(CHTLTokenType::AT_CONFIG)) {
        useType = "@config";
        advance();
    } else {
        addError("期望use类型标识符");
        return nullptr;
    }
    
    pushState(CHTLParsingState::InUseStatement);
    auto useNode = CHTLNodeFactory::createUseDeclaration(useType);
    popState();
    
    return useNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseNamespaceDeclaration() {
    consume(CHTLTokenType::NAMESPACE, "namespace");
    
    if (!check(CHTLTokenType::IDENTIFIER)) {
        addError("期望命名空间名称");
        return nullptr;
    }
    
    std::string namespaceName = advance().value;
    enterNamespace(namespaceName);
    
    auto namespaceNode = CHTLNodeFactory::createNamespaceDeclaration(namespaceName);
    
    // 解析命名空间体
    if (match(CHTLTokenType::LBRACE)) {
        pushState(CHTLParsingState::InNamespaceDefinition);
        
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            if (match(CHTLTokenType::WHITESPACE) || match(CHTLTokenType::NEWLINE)) {
                continue;
            }
            
            auto declaration = parseDeclaration();
            if (declaration) {
                namespaceNode->addChild(declaration);
            }
        }
        
        consume(CHTLTokenType::RBRACE, "}");
        popState();
    }
    
    exitNamespace();
    return namespaceNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseImportStatement() {
    consume(CHTLTokenType::IMPORT, "import");
    
    // 解析导入类型
    std::string importType;
    if (check(CHTLTokenType::AT_STYLE)) {
        importType = "@style";
        advance();
    } else if (check(CHTLTokenType::AT_ELEMENT)) {
        importType = "@element";
        advance();
    } else if (check(CHTLTokenType::AT_VAR)) {
        importType = "@var";
        advance();
    } else if (check(CHTLTokenType::AT_HTML)) {
        importType = "@html";
        advance();
    } else if (check(CHTLTokenType::AT_JAVASCRIPT)) {
        importType = "@javascript";
        advance();
    } else if (check(CHTLTokenType::AT_CHTL)) {
        importType = "@chtl";
        advance();
    } else if (check(CHTLTokenType::AT_CJMOD)) {
        importType = "@cjmod";
        advance();
    } else if (check(CHTLTokenType::AT_CONFIG)) {
        importType = "@config";
        advance();
    } else {
        addError("期望导入类型标识符");
        return nullptr;
    }
    
    pushState(CHTLParsingState::InImportStatement);
    
    // 解析导入名称或路径
    std::string importPath;
    if (check(CHTLTokenType::STRING_LITERAL)) {
        importPath = advance().value;
    } else if (check(CHTLTokenType::IDENTIFIER)) {
        importPath = advance().value;
    } else {
        addError("期望导入路径或名称");
        popState();
        return nullptr;
    }
    
    auto importNode = CHTLNodeFactory::createImport(importType, importPath);
    
    // 解析from子句
    if (match(CHTLTokenType::FROM)) {
        if (!check(CHTLTokenType::STRING_LITERAL)) {
            addError("期望from路径");
        } else {
            std::string fromPath = advance().value;
            importNode->setFromPath(fromPath);
        }
    }
    
    // 解析as子句
    if (match(CHTLTokenType::AS)) {
        if (!check(CHTLTokenType::IDENTIFIER)) {
            addError("期望别名");
        } else {
            std::string alias = advance().value;
            importNode->setAlias(alias);
        }
    }
    
    // 解析except子句
    if (match(CHTLTokenType::EXCEPT)) {
        pushState(CHTLParsingState::InExceptClause);
        
        if (match(CHTLTokenType::LBRACE)) {
            while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
                if (match(CHTLTokenType::WHITESPACE) || match(CHTLTokenType::NEWLINE)) {
                    continue;
                }
                
                if (check(CHTLTokenType::IDENTIFIER)) {
                    importNode->addExceptItem(advance().value);
                    match(CHTLTokenType::COMMA);
                } else {
                    addError("期望排除项名称");
                    break;
                }
            }
            consume(CHTLTokenType::RBRACE, "}");
        }
        
        popState();
    }
    
    popState();
    return importNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseConfigurationDefinition() {
    consume(CHTLTokenType::CONFIGURATION, "configuration");
    
    auto configNode = CHTLNodeFactory::createConfigurationDefinition();
    
    if (match(CHTLTokenType::LBRACE)) {
        pushState(CHTLParsingState::InConfigurationDefinition);
        
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            if (match(CHTLTokenType::WHITESPACE) || match(CHTLTokenType::NEWLINE)) {
                continue;
            }
            
            auto configBlock = parseConfigurationBlock();
            if (configBlock) {
                configNode->addChild(configBlock);
            }
        }
        
        consume(CHTLTokenType::RBRACE, "}");
        popState();
    }
    
    return configNode;
}

// === 模板系统解析 ===

std::shared_ptr<CHTLTemplateNode> CHTLParser::parseTemplateDefinition() {
    consume(CHTLTokenType::TEMPLATE, "template");
    
    // 解析模板类型
    std::string templateType;
    if (check(CHTLTokenType::AT_STYLE)) {
        templateType = "@style";
        advance();
    } else if (check(CHTLTokenType::AT_ELEMENT)) {
        templateType = "@element";
        advance();
    } else if (check(CHTLTokenType::AT_VAR)) {
        templateType = "@var";
        advance();
    } else {
        addError("期望模板类型");
        return nullptr;
    }
    
    // 解析模板名称
    if (!check(CHTLTokenType::IDENTIFIER)) {
        addError("期望模板名称");
        return nullptr;
    }
    
    std::string templateName = advance().value;
    enterTemplate(templateName);
    
    auto templateNode = CHTLNodeFactory::createTemplate(templateName, templateType);
    
    // 解析继承
    if (match(CHTLTokenType::INHERIT)) {
        auto inheritNode = parseTemplateInheritance();
        if (inheritNode) {
            templateNode->addChild(inheritNode);
        }
    }
    
    // 解析模板体
    if (match(CHTLTokenType::LBRACE)) {
        pushState(CHTLParsingState::InTemplateDefinition);
        
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            if (match(CHTLTokenType::WHITESPACE) || match(CHTLTokenType::NEWLINE)) {
                continue;
            }
            
            auto content = parseDeclaration();
            if (content) {
                templateNode->addChild(content);
            }
        }
        
        consume(CHTLTokenType::RBRACE, "}");
        popState();
    }
    
    exitTemplate();
    return templateNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseTemplateInheritance() {
    auto inheritNode = std::make_shared<CHTLNode>(CHTLNodeType::INHERIT_STATEMENT, "inherit");
    
    // 解析父模板类型和名称
    std::string parentType;
    if (check(CHTLTokenType::AT_STYLE)) {
        parentType = "@style";
        advance();
    } else if (check(CHTLTokenType::AT_ELEMENT)) {
        parentType = "@element";
        advance();
    } else if (check(CHTLTokenType::AT_VAR)) {
        parentType = "@var";
        advance();
    } else {
        addError("期望父模板类型");
        return nullptr;
    }
    
    if (!check(CHTLTokenType::IDENTIFIER)) {
        addError("期望父模板名称");
        return nullptr;
    }
    
    std::string parentName = advance().value;
    inheritNode->setAttribute("parent_type", parentType);
    inheritNode->setAttribute("parent_name", parentName);
    
    return inheritNode;
}

// === 自定义系统解析 ===

std::shared_ptr<CHTLCustomNode> CHTLParser::parseCustomDefinition() {
    consume(CHTLTokenType::CUSTOM, "custom");
    
    // 解析自定义类型
    std::string customType;
    if (check(CHTLTokenType::AT_STYLE)) {
        customType = "@style";
        advance();
    } else if (check(CHTLTokenType::AT_ELEMENT)) {
        customType = "@element";
        advance();
    } else if (check(CHTLTokenType::AT_VAR)) {
        customType = "@var";
        advance();
    } else {
        addError("期望自定义类型");
        return nullptr;
    }
    
    // 解析自定义名称
    if (!check(CHTLTokenType::IDENTIFIER)) {
        addError("期望自定义名称");
        return nullptr;
    }
    
    std::string customName = advance().value;
    enterCustom(customName);
    
    auto customNode = CHTLNodeFactory::createCustom(customName, customType);
    
    // 解析自定义体
    if (match(CHTLTokenType::LBRACE)) {
        pushState(CHTLParsingState::InCustomDefinition);
        
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            if (match(CHTLTokenType::WHITESPACE) || match(CHTLTokenType::NEWLINE)) {
                continue;
            }
            
            // 解析delete, insert, inherit语句
            if (check(CHTLTokenType::DELETE)) {
                auto deleteNode = parseDeleteStatement();
                if (deleteNode) customNode->addChild(deleteNode);
            } else if (check(CHTLTokenType::INSERT)) {
                auto insertNode = parseInsertStatement();
                if (insertNode) customNode->addChild(insertNode);
            } else if (check(CHTLTokenType::INHERIT)) {
                auto inheritNode = parseInheritStatement();
                if (inheritNode) customNode->addChild(inheritNode);
            } else {
                auto content = parseDeclaration();
                if (content) customNode->addChild(content);
            }
        }
        
        consume(CHTLTokenType::RBRACE, "}");
        popState();
    }
    
    exitCustom();
    return customNode;
}

// === HTML元素解析 ===

std::shared_ptr<CHTLElementNode> CHTLParser::parseElement() {
    std::string elementName;
    
    // 解析元素名称或模板/自定义使用
    if (check(CHTLTokenType::IDENTIFIER)) {
        elementName = advance().value;
    } else if (check(CHTLTokenType::AT_STYLE) || check(CHTLTokenType::AT_ELEMENT) || check(CHTLTokenType::AT_VAR)) {
        // 模板/自定义使用
        return parseTemplateUsage();
    } else {
        addError("期望元素名称");
        return nullptr;
    }
    
    auto element = CHTLNodeFactory::createElement(elementName);
    
    // 解析属性
    if (match(CHTLTokenType::LPAREN)) {
        pushState(CHTLParsingState::InAttributeList);
        
        while (!check(CHTLTokenType::RPAREN) && !isAtEnd()) {
            if (match(CHTLTokenType::WHITESPACE) || match(CHTLTokenType::NEWLINE)) {
                continue;
            }
            
            if (check(CHTLTokenType::IDENTIFIER)) {
                std::string attrName = advance().value;
                std::string attrValue;
                
                if (match(CHTLTokenType::EQUAL) || match(CHTLTokenType::COLON)) {
                    if (check(CHTLTokenType::STRING_LITERAL)) {
                        attrValue = advance().value;
                    } else if (check(CHTLTokenType::UNQUOTED_LITERAL)) {
                        attrValue = advance().value;
                    } else if (check(CHTLTokenType::IDENTIFIER)) {
                        attrValue = advance().value;
                    } else {
                        addError("期望属性值");
                    }
                }
                
                element->setAttribute(attrName, attrValue);
                match(CHTLTokenType::COMMA);
            } else {
                addError("期望属性名称");
                break;
            }
        }
        
        consume(CHTLTokenType::RPAREN, ")");
        popState();
    }
    
    // 解析元素体
    if (match(CHTLTokenType::LBRACE)) {
        pushState(CHTLParsingState::InElementBody);
        
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            if (match(CHTLTokenType::WHITESPACE) || match(CHTLTokenType::NEWLINE)) {
                continue;
            }
            
            // 解析样式块
            if (check(CHTLTokenType::STYLE)) {
                auto styleNode = parseStyleBlock();
                if (styleNode) element->addChild(styleNode);
            }
            // 解析脚本块
            else if (check(CHTLTokenType::SCRIPT)) {
                auto scriptNode = parseScriptBlock();
                if (scriptNode) element->addChild(scriptNode);
            }
            // 解析子元素或文本
            else {
                auto content = parseDeclaration();
                if (content) element->addChild(content);
            }
        }
        
        consume(CHTLTokenType::RBRACE, "}");
        popState();
    }
    
    return element;
}

std::shared_ptr<CHTLNode> CHTLParser::parseTextNode() {
    std::string text;
    
    if (check(CHTLTokenType::STRING_LITERAL)) {
        text = advance().value;
    } else if (check(CHTLTokenType::UNQUOTED_LITERAL)) {
        text = advance().value;
    } else {
        addError("期望文本内容");
        return nullptr;
    }
    
    return CHTLNodeFactory::createTextNode(text);
}

// === 样式解析 ===

std::shared_ptr<CHTLStyleNode> CHTLParser::parseStyleBlock() {
    consume(CHTLTokenType::STYLE, "style");
    
    bool isLocal = false;
    // 检查是否为本地样式
    if (getCurrentState() == CHTLParsingState::InElementBody) {
        isLocal = true;
    }
    
    auto styleNode = CHTLNodeFactory::createStyle(isLocal);
    
    if (match(CHTLTokenType::LBRACE)) {
        pushState(CHTLParsingState::InStyleBlock);
        
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            if (match(CHTLTokenType::WHITESPACE) || match(CHTLTokenType::NEWLINE)) {
                continue;
            }
            
            // 解析选择器和规则
            if (check(CHTLTokenType::CLASS_SELECTOR) || check(CHTLTokenType::ID_SELECTOR) || 
                check(CHTLTokenType::IDENTIFIER) || check(CHTLTokenType::AMPERSAND)) {
                auto rule = parseStyleRule();
                if (rule) styleNode->addChild(rule);
            }
            // 解析直接属性（无选择器）
            else if (check(CHTLTokenType::IDENTIFIER)) {
                auto property = parseStyleProperty();
                if (property) styleNode->addChild(property);
            } else {
                addError("期望样式选择器或属性");
                advance();
            }
        }
        
        consume(CHTLTokenType::RBRACE, "}");
        popState();
    }
    
    return styleNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseStyleRule() {
    // 解析选择器
    std::string selector;
    
    if (check(CHTLTokenType::CLASS_SELECTOR)) {
        selector = advance().value;
    } else if (check(CHTLTokenType::ID_SELECTOR)) {
        selector = advance().value;
    } else if (check(CHTLTokenType::AMPERSAND)) {
        selector = advance().value;
    } else if (check(CHTLTokenType::IDENTIFIER)) {
        selector = advance().value;
    } else {
        addError("期望选择器");
        return nullptr;
    }
    
    auto rule = std::make_shared<CHTLNode>(CHTLNodeType::STYLE_RULE, selector);
    
    if (match(CHTLTokenType::LBRACE)) {
        pushState(CHTLParsingState::InStyleSelector);
        
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            if (match(CHTLTokenType::WHITESPACE) || match(CHTLTokenType::NEWLINE)) {
                continue;
            }
            
            auto property = parseStyleProperty();
            if (property) rule->addChild(property);
        }
        
        consume(CHTLTokenType::RBRACE, "}");
        popState();
    }
    
    return rule;
}

std::shared_ptr<CHTLNode> CHTLParser::parseStyleProperty() {
    if (!check(CHTLTokenType::IDENTIFIER)) {
        addError("期望样式属性名");
        return nullptr;
    }
    
    std::string propertyName = advance().value;
    
    if (!match(CHTLTokenType::COLON) && !match(CHTLTokenType::EQUAL)) {
        addError("期望 : 或 =");
        return nullptr;
    }
    
    std::string propertyValue;
    if (check(CHTLTokenType::STRING_LITERAL)) {
        propertyValue = advance().value;
    } else if (check(CHTLTokenType::UNQUOTED_LITERAL)) {
        propertyValue = advance().value;
    } else if (check(CHTLTokenType::IDENTIFIER)) {
        propertyValue = advance().value;
    } else if (check(CHTLTokenType::NUMBER)) {
        propertyValue = advance().value;
    } else {
        addError("期望属性值");
        return nullptr;
    }
    
    match(CHTLTokenType::SEMICOLON);
    
    auto property = std::make_shared<CHTLNode>(CHTLNodeType::STYLE_PROPERTY, propertyName, propertyValue);
    return property;
}

// === 脚本解析 ===

std::shared_ptr<CHTLScriptNode> CHTLParser::parseScriptBlock() {
    consume(CHTLTokenType::SCRIPT, "script");
    
    bool isLocal = false;
    if (getCurrentState() == CHTLParsingState::InElementBody) {
        isLocal = true;
    }
    
    auto scriptNode = CHTLNodeFactory::createScript(isLocal);
    
    if (match(CHTLTokenType::LBRACE)) {
        pushState(CHTLParsingState::InScriptBlock);
        
        std::string scriptContent;
        while (!check(CHTLTokenType::RBRACE) && !isAtEnd()) {
            CHTLToken token = advance();
            scriptContent += token.value + " ";
        }
        
        scriptNode->setScriptContent(scriptContent);
        
        consume(CHTLTokenType::RBRACE, "}");
        popState();
    }
    
    return scriptNode;
}

// === 注释解析 ===

std::shared_ptr<CHTLNode> CHTLParser::parseComment() {
    if (check(CHTLTokenType::LINE_COMMENT)) {
        std::string comment = advance().value;
        return CHTLNodeFactory::createLineComment(comment);
    } else if (check(CHTLTokenType::BLOCK_COMMENT)) {
        std::string comment = advance().value;
        return CHTLNodeFactory::createBlockComment(comment);
    } else if (check(CHTLTokenType::GENERATOR_COMMENT)) {
        std::string comment = advance().value;
        return CHTLNodeFactory::createGeneratorComment(comment);
    }
    
    return nullptr;
}

// === 辅助方法实现 ===

bool CHTLParser::isComment() const {
    return check(CHTLTokenType::LINE_COMMENT) || 
           check(CHTLTokenType::BLOCK_COMMENT) || 
           check(CHTLTokenType::GENERATOR_COMMENT);
}

bool CHTLParser::isElement() const {
    return check(CHTLTokenType::IDENTIFIER) && 
           validateHTMLElement(peek().value);
}

bool CHTLParser::isTemplateUsage() const {
    return (check(CHTLTokenType::AT_STYLE) || 
            check(CHTLTokenType::AT_ELEMENT) || 
            check(CHTLTokenType::AT_VAR)) &&
           peek(1).type == CHTLTokenType::IDENTIFIER;
}

bool CHTLParser::isCustomUsage() const {
    return isTemplateUsage(); // 自定义使用与模板使用语法相同
}

// === 上下文管理 ===

void CHTLParser::enterNamespace(const std::string& namespaceName) {
    currentNamespace = namespaceName;
}

void CHTLParser::exitNamespace() {
    currentNamespace.clear();
}

void CHTLParser::enterTemplate(const std::string& templateName) {
    currentTemplate = templateName;
}

void CHTLParser::exitTemplate() {
    currentTemplate.clear();
}

void CHTLParser::enterCustom(const std::string& customName) {
    currentCustom = customName;
}

void CHTLParser::exitCustom() {
    currentCustom.clear();
}

// === 验证方法 ===

bool CHTLParser::validateHTMLElement(const std::string& elementName) const {
    const auto& builtinTags = getBuiltinHTMLTags();
    return std::find(builtinTags.begin(), builtinTags.end(), elementName) != builtinTags.end();
}

const std::vector<std::string>& CHTLParser::getBuiltinHTMLTags() {
    static std::vector<std::string> tags = {
        "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "a", "img", "ul", "ol", "li", "table", "tr", "td", "th",
        "form", "input", "button", "select", "option", "textarea",
        "header", "nav", "main", "section", "article", "aside", "footer",
        "strong", "em", "small", "mark", "del", "ins", "sub", "sup"
    };
    return tags;
}

// === 未实现的方法存根 ===

std::shared_ptr<CHTLNode> CHTLParser::parseTemplateUsage() {
    // TODO: 实现模板使用解析
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseOriginDefinition() {
    // TODO: 实现原始嵌入定义解析
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseDeleteStatement() {
    // TODO: 实现delete语句解析
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseInsertStatement() {
    // TODO: 实现insert语句解析
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseInheritStatement() {
    // TODO: 实现inherit语句解析
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseConfigurationBlock() {
    // TODO: 实现配置块解析
    return nullptr;
}

} // namespace CHTL