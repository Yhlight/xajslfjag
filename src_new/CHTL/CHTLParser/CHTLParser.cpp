#include "CHTLParser.h"
#include "../CHTLLexer/Lexer.h"
#include "../../Error/ErrorReporter.h"
#include <iostream>
#include <algorithm>

namespace CHTL {

CHTLParser::CHTLParser() 
    : m_currentTokenIndex(0), m_debugMode(false), m_strictMode(true) {
    m_context = std::make_shared<CHTLContext>();
}

CHTLParser::~CHTLParser() = default;

std::shared_ptr<BaseNode> CHTLParser::parse(const std::string& code) {
    try {
        if (m_debugMode) {
            std::cout << "🔍 开始CHTL解析，代码长度: " << code.length() << std::endl;
        }
        
        // 词法分析
        Lexer lexer;
        m_tokens = lexer.tokenize(code);
        m_currentTokenIndex = 0;
        
        if (m_debugMode) {
            std::cout << "  ✓ 词法分析完成，Token数量: " << m_tokens.size() << std::endl;
        }
        
        // 语法分析
        auto rootNode = parseDocument();
        
        if (m_debugMode) {
            std::cout << "  ✓ 语法分析完成" << std::endl;
        }
        
        return rootNode;
        
    } catch (const std::exception& e) {
        reportError("解析过程中发生异常: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<BaseNode> CHTLParser::parse(const std::vector<Token>& tokens) {
    m_tokens = tokens;
    m_currentTokenIndex = 0;
    return parseDocument();
}

std::shared_ptr<BaseNode> CHTLParser::parseDocument() {
    auto documentNode = std::make_shared<BaseNode>(NodeType::DOCUMENT);
    documentNode->setValue("document");
    
    while (hasMoreTokens()) {
        Token token = getCurrentToken();
        
        if (token.type == TokenType::EOF_TOKEN) {
            break;
        }
        
        std::shared_ptr<BaseNode> childNode = nullptr;
        
        // 根据Token类型决定解析方法
        switch (token.type) {
            case TokenType::BRACKET_OPEN:
                // 可能是Template, Custom, Origin, Import, Namespace, Configuration
                childNode = parseBlockStatement();
                break;
                
            case TokenType::IDENTIFIER:
                // 可能是HTML元素
                childNode = parseElement();
                break;
                
            case TokenType::COMMENT_GENERATOR:
                childNode = parseGeneratorComment();
                break;
                
            case TokenType::COMMENT_NORMAL:
                // 跳过普通注释
                consumeToken();
                continue;
                
            default:
                if (m_strictMode) {
                    reportError("意外的Token类型: " + std::to_string(static_cast<int>(token.type)));
                }
                consumeToken();
                continue;
        }
        
        if (childNode) {
            documentNode->addChild(childNode);
        }
    }
    
    return documentNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseBlockStatement() {
    Token token = getCurrentToken();
    if (token.type != TokenType::BRACKET_OPEN) {
        reportError("期望 '['");
        return nullptr;
    }
    
    consumeToken();  // 消费 '['
    
    Token blockType = getCurrentToken();
    if (blockType.type != TokenType::IDENTIFIER) {
        reportError("期望块类型标识符");
        return nullptr;
    }
    
    std::string blockTypeName = getTokenValue(blockType);
    
    if (blockTypeName == "Template") {
        return parseTemplate();
    } else if (blockTypeName == "Custom") {
        return parseCustom();
    } else if (blockTypeName == "Origin") {
        return parseOrigin();
    } else if (blockTypeName == "Import") {
        return parseImport();
    } else if (blockTypeName == "Namespace") {
        return parseNamespace();
    } else if (blockTypeName == "Configuration") {
        return parseConfiguration();
    } else {
        reportError("未知的块类型: " + blockTypeName);
        return nullptr;
    }
}

std::shared_ptr<BaseNode> CHTLParser::parseTemplate() {
    auto templateNode = std::make_shared<BaseNode>(NodeType::TEMPLATE);
    
    consumeToken();  // 消费 'Template'
    
    if (!expectToken(TokenType::BRACKET_CLOSE)) {
        return nullptr;
    }
    
    // 解析模板类型 @Style, @Element, @Var
    if (!expectToken(TokenType::AT)) {
        return nullptr;
    }
    
    Token typeToken = getCurrentToken();
    if (typeToken.type != TokenType::IDENTIFIER) {
        reportError("期望模板类型");
        return nullptr;
    }
    
    std::string templateType = getTokenValue(typeToken);
    templateNode->setAttribute("type", templateType);
    consumeToken();
    
    // 解析模板名称
    Token nameToken = getCurrentToken();
    if (nameToken.type != TokenType::IDENTIFIER) {
        reportError("期望模板名称");
        return nullptr;
    }
    
    std::string templateName = getTokenValue(nameToken);
    templateNode->setAttribute("name", templateName);
    templateNode->setValue(templateName);
    consumeToken();
    
    // 解析继承
    if (matchToken(TokenType::IDENTIFIER) && getTokenValue(getCurrentToken()) == "from") {
        consumeToken();  // 消费 'from'
        
        Token parentToken = getCurrentToken();
        if (parentToken.type == TokenType::IDENTIFIER) {
            templateNode->setAttribute("inherit", getTokenValue(parentToken));
            consumeToken();
        }
    }
    
    // 解析模板体
    if (expectToken(TokenType::BRACE_OPEN)) {
        while (hasMoreTokens() && !matchToken(TokenType::BRACE_CLOSE)) {
            auto childNode = parseTemplateContent(templateType);
            if (childNode) {
                templateNode->addChild(childNode);
            }
        }
        
        if (!expectToken(TokenType::BRACE_CLOSE)) {
            return nullptr;
        }
    }
    
    if (m_debugMode) {
        std::cout << "  ✓ 解析模板: " << templateType << " " << templateName << std::endl;
    }
    
    return templateNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseTemplateContent(const std::string& templateType) {
    if (templateType == "Style") {
        return parseStyleContent();
    } else if (templateType == "Element") {
        return parseElement();
    } else if (templateType == "Var") {
        return parseVariableDefinition();
    }
    
    // 默认解析
    return parseElement();
}

std::shared_ptr<BaseNode> CHTLParser::parseStyleContent() {
    auto styleNode = std::make_shared<BaseNode>(NodeType::STYLE);
    
    // 解析CSS属性
    while (hasMoreTokens() && !matchToken(TokenType::BRACE_CLOSE)) {
        Token propertyToken = getCurrentToken();
        if (propertyToken.type != TokenType::IDENTIFIER) {
            break;
        }
        
        std::string property = getTokenValue(propertyToken);
        consumeToken();
        
        if (!expectToken(TokenType::COLON)) {
            break;
        }
        
        Token valueToken = getCurrentToken();
        std::string value = getTokenValue(valueToken);
        consumeToken();
        
        if (expectToken(TokenType::SEMICOLON)) {
            styleNode->setAttribute(property, value);
        }
    }
    
    return styleNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseVariableDefinition() {
    auto varNode = std::make_shared<BaseNode>(NodeType::VARIABLE);
    
    while (hasMoreTokens() && !matchToken(TokenType::BRACE_CLOSE)) {
        Token nameToken = getCurrentToken();
        if (nameToken.type != TokenType::IDENTIFIER) {
            break;
        }
        
        std::string varName = getTokenValue(nameToken);
        consumeToken();
        
        if (!expectToken(TokenType::COLON)) {
            break;
        }
        
        Token valueToken = getCurrentToken();
        std::string varValue = getTokenValue(valueToken);
        consumeToken();
        
        if (expectToken(TokenType::SEMICOLON)) {
            varNode->setAttribute(varName, varValue);
        }
    }
    
    return varNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseCustom() {
    auto customNode = std::make_shared<BaseNode>(NodeType::CUSTOM);
    
    // 类似于parseTemplate的实现
    consumeToken();  // 消费 'Custom'
    
    if (!expectToken(TokenType::BRACKET_CLOSE)) {
        return nullptr;
    }
    
    // 解析自定义类型和名称
    if (!expectToken(TokenType::AT)) {
        return nullptr;
    }
    
    Token typeToken = getCurrentToken();
    std::string customType = getTokenValue(typeToken);
    customNode->setAttribute("type", customType);
    consumeToken();
    
    Token nameToken = getCurrentToken();
    std::string customName = getTokenValue(nameToken);
    customNode->setAttribute("name", customName);
    customNode->setValue(customName);
    consumeToken();
    
    // 解析继承
    if (matchToken(TokenType::IDENTIFIER) && getTokenValue(getCurrentToken()) == "from") {
        consumeToken();
        Token parentToken = getCurrentToken();
        customNode->setAttribute("inherit", getTokenValue(parentToken));
        consumeToken();
    }
    
    // 解析自定义体
    if (expectToken(TokenType::BRACE_OPEN)) {
        while (hasMoreTokens() && !matchToken(TokenType::BRACE_CLOSE)) {
            auto childNode = parseCustomContent(customType);
            if (childNode) {
                customNode->addChild(childNode);
            }
        }
        
        expectToken(TokenType::BRACE_CLOSE);
    }
    
    return customNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseElement() {
    Token elementToken = getCurrentToken();
    if (elementToken.type != TokenType::IDENTIFIER) {
        reportError("期望元素名称");
        return nullptr;
    }
    
    auto elementNode = std::make_shared<BaseNode>(NodeType::ELEMENT);
    std::string elementName = getTokenValue(elementToken);
    elementNode->setValue(elementName);
    elementNode->setAttribute("tag", elementName);
    consumeToken();
    
    if (expectToken(TokenType::BRACE_OPEN)) {
        while (hasMoreTokens() && !matchToken(TokenType::BRACE_CLOSE)) {
            auto childNode = parseElementContent();
            if (childNode) {
                elementNode->addChild(childNode);
            }
        }
        
        expectToken(TokenType::BRACE_CLOSE);
    }
    
    return elementNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseElementContent() {
    Token token = getCurrentToken();
    
    if (token.type == TokenType::IDENTIFIER) {
        std::string value = getTokenValue(token);
        
        if (value == "text") {
            return parseTextNode();
        } else if (value == "style") {
            return parseStyleBlock();
        } else if (value == "script") {
            return parseScriptBlock();
        } else {
            // 检查是否是属性
            Token nextToken = peekToken();
            if (nextToken.type == TokenType::COLON) {
                return parseAttribute();
            } else {
                // 子元素
                return parseElement();
            }
        }
    }
    
    consumeToken();  // 跳过无法识别的Token
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseAttribute() {
    Token nameToken = getCurrentToken();
    std::string attrName = getTokenValue(nameToken);
    consumeToken();
    
    if (!expectToken(TokenType::COLON)) {
        return nullptr;
    }
    
    Token valueToken = getCurrentToken();
    std::string attrValue = getTokenValue(valueToken);
    consumeToken();
    
    expectToken(TokenType::SEMICOLON);
    
    auto attrNode = std::make_shared<BaseNode>(NodeType::ATTRIBUTE);
    attrNode->setAttribute("name", attrName);
    attrNode->setAttribute("value", attrValue);
    attrNode->setValue(attrValue);
    
    return attrNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseTextNode() {
    consumeToken();  // 消费 'text'
    
    auto textNode = std::make_shared<BaseNode>(NodeType::TEXT);
    
    if (expectToken(TokenType::BRACE_OPEN)) {
        // 收集文本内容
        std::string textContent;
        
        while (hasMoreTokens() && !matchToken(TokenType::BRACE_CLOSE)) {
            Token contentToken = getCurrentToken();
            textContent += getTokenValue(contentToken) + " ";
            consumeToken();
        }
        
        expectToken(TokenType::BRACE_CLOSE);
        
        // 清理多余空格
        textContent.erase(textContent.find_last_not_of(" \t\n\r") + 1);
        textNode->setValue(textContent);
    }
    
    return textNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseStyleBlock() {
    consumeToken();  // 消费 'style'
    
    auto styleNode = std::make_shared<BaseNode>(NodeType::STYLE);
    
    if (expectToken(TokenType::BRACE_OPEN)) {
        while (hasMoreTokens() && !matchToken(TokenType::BRACE_CLOSE)) {
            // 解析CSS内容
            auto cssNode = parseStyleContent();
            if (cssNode) {
                styleNode->addChild(cssNode);
            }
        }
        
        expectToken(TokenType::BRACE_CLOSE);
    }
    
    return styleNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseScriptBlock() {
    consumeToken();  // 消费 'script'
    
    auto scriptNode = std::make_shared<BaseNode>(NodeType::SCRIPT);
    
    if (expectToken(TokenType::BRACE_OPEN)) {
        // 收集脚本内容
        std::string scriptContent;
        int braceDepth = 1;
        
        while (hasMoreTokens() && braceDepth > 0) {
            Token contentToken = getCurrentToken();
            
            if (contentToken.type == TokenType::BRACE_OPEN) {
                braceDepth++;
            } else if (contentToken.type == TokenType::BRACE_CLOSE) {
                braceDepth--;
            }
            
            if (braceDepth > 0) {
                scriptContent += getTokenValue(contentToken) + " ";
            }
            
            consumeToken();
        }
        
        scriptNode->setValue(scriptContent);
    }
    
    return scriptNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseOrigin() {
    auto originNode = std::make_shared<BaseNode>(NodeType::ORIGIN);
    
    consumeToken();  // 消费 'Origin'
    
    if (!expectToken(TokenType::BRACKET_CLOSE)) {
        return nullptr;
    }
    
    // 解析Origin类型
    if (!expectToken(TokenType::AT)) {
        return nullptr;
    }
    
    Token typeToken = getCurrentToken();
    std::string originType = getTokenValue(typeToken);
    originNode->setAttribute("type", originType);
    consumeToken();
    
    // 解析可选的名称
    if (matchToken(TokenType::IDENTIFIER)) {
        Token nameToken = getCurrentToken();
        std::string originName = getTokenValue(nameToken);
        originNode->setAttribute("name", originName);
        originNode->setValue(originName);
        consumeToken();
    }
    
    // 解析Origin内容
    if (expectToken(TokenType::BRACE_OPEN)) {
        std::string originContent;
        int braceDepth = 1;
        
        while (hasMoreTokens() && braceDepth > 0) {
            Token contentToken = getCurrentToken();
            
            if (contentToken.type == TokenType::BRACE_OPEN) {
                braceDepth++;
            } else if (contentToken.type == TokenType::BRACE_CLOSE) {
                braceDepth--;
            }
            
            if (braceDepth > 0) {
                originContent += getTokenValue(contentToken);
            }
            
            consumeToken();
        }
        
        originNode->setValue(originContent);
    }
    
    return originNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseImport() {
    auto importNode = std::make_shared<BaseNode>(NodeType::IMPORT);
    
    consumeToken();  // 消费 'Import'
    
    if (!expectToken(TokenType::BRACKET_CLOSE)) {
        return nullptr;
    }
    
    // 解析导入类型
    if (matchToken(TokenType::AT)) {
        consumeToken();
        Token typeToken = getCurrentToken();
        std::string importType = getTokenValue(typeToken);
        importNode->setAttribute("type", importType);
        consumeToken();
    } else if (matchToken(TokenType::BRACKET_OPEN)) {
        // 解析复杂导入类型 [Template] @Style等
        consumeToken();
        Token categoryToken = getCurrentToken();
        std::string category = getTokenValue(categoryToken);
        consumeToken();
        
        expectToken(TokenType::BRACKET_CLOSE);
        expectToken(TokenType::AT);
        
        Token typeToken = getCurrentToken();
        std::string type = getTokenValue(typeToken);
        importNode->setAttribute("category", category);
        importNode->setAttribute("type", type);
        consumeToken();
    }
    
    // 解析from子句
    if (matchToken(TokenType::IDENTIFIER) && getTokenValue(getCurrentToken()) == "from") {
        consumeToken();
        
        Token pathToken = getCurrentToken();
        std::string importPath = getTokenValue(pathToken);
        importNode->setAttribute("path", importPath);
        consumeToken();
        
        // 解析可选的as子句
        if (matchToken(TokenType::IDENTIFIER) && getTokenValue(getCurrentToken()) == "as") {
            consumeToken();
            
            Token aliasToken = getCurrentToken();
            std::string alias = getTokenValue(aliasToken);
            importNode->setAttribute("alias", alias);
            consumeToken();
        }
    }
    
    return importNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseNamespace() {
    auto namespaceNode = std::make_shared<BaseNode>(NodeType::NAMESPACE);
    
    consumeToken();  // 消费 'Namespace'
    
    if (!expectToken(TokenType::BRACKET_CLOSE)) {
        return nullptr;
    }
    
    // 解析命名空间名称
    Token nameToken = getCurrentToken();
    if (nameToken.type != TokenType::IDENTIFIER) {
        reportError("期望命名空间名称");
        return nullptr;
    }
    
    std::string namespaceName = getTokenValue(nameToken);
    namespaceNode->setValue(namespaceName);
    namespaceNode->setAttribute("name", namespaceName);
    consumeToken();
    
    // 解析命名空间内容
    if (expectToken(TokenType::BRACE_OPEN)) {
        while (hasMoreTokens() && !matchToken(TokenType::BRACE_CLOSE)) {
            auto childNode = parseNamespaceContent();
            if (childNode) {
                namespaceNode->addChild(childNode);
            }
        }
        
        expectToken(TokenType::BRACE_CLOSE);
    }
    
    return namespaceNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseConfiguration() {
    auto configNode = std::make_shared<BaseNode>(NodeType::CONFIGURATION);
    
    consumeToken();  // 消费 'Configuration'
    
    if (!expectToken(TokenType::BRACKET_CLOSE)) {
        return nullptr;
    }
    
    // 解析可选的配置名称
    if (matchToken(TokenType::AT)) {
        consumeToken();
        expectToken(TokenType::IDENTIFIER);  // Config
        
        Token nameToken = peekToken();
        if (nameToken.type == TokenType::IDENTIFIER) {
            consumeToken();  // 消费 'Config'
            
            Token configNameToken = getCurrentToken();
            std::string configName = getTokenValue(configNameToken);
            configNode->setAttribute("name", configName);
            configNode->setValue(configName);
            consumeToken();
        }
    }
    
    // 解析配置内容
    if (expectToken(TokenType::BRACE_OPEN)) {
        while (hasMoreTokens() && !matchToken(TokenType::BRACE_CLOSE)) {
            auto childNode = parseConfigurationContent();
            if (childNode) {
                configNode->addChild(childNode);
            }
        }
        
        expectToken(TokenType::BRACE_CLOSE);
    }
    
    return configNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseGeneratorComment() {
    Token commentToken = getCurrentToken();
    
    auto commentNode = std::make_shared<BaseNode>(NodeType::COMMENT);
    commentNode->setAttribute("type", "generator");
    commentNode->setValue(getTokenValue(commentToken));
    
    consumeToken();
    
    return commentNode;
}

// Token操作方法
Token CHTLParser::getCurrentToken() const {
    if (m_currentTokenIndex < m_tokens.size()) {
        return m_tokens[m_currentTokenIndex];
    }
    
    Token eofToken;
    eofToken.type = TokenType::EOF_TOKEN;
    return eofToken;
}

Token CHTLParser::getNextToken() {
    if (m_currentTokenIndex + 1 < m_tokens.size()) {
        return m_tokens[m_currentTokenIndex + 1];
    }
    
    Token eofToken;
    eofToken.type = TokenType::EOF_TOKEN;
    return eofToken;
}

Token CHTLParser::peekToken(size_t offset) const {
    size_t index = m_currentTokenIndex + offset;
    if (index < m_tokens.size()) {
        return m_tokens[index];
    }
    
    Token eofToken;
    eofToken.type = TokenType::EOF_TOKEN;
    return eofToken;
}

bool CHTLParser::hasMoreTokens() const {
    return m_currentTokenIndex < m_tokens.size();
}

void CHTLParser::consumeToken() {
    if (m_currentTokenIndex < m_tokens.size()) {
        m_currentTokenIndex++;
    }
}

bool CHTLParser::expectToken(TokenType type) {
    Token token = getCurrentToken();
    if (token.type == type) {
        consumeToken();
        return true;
    }
    
    reportError("期望Token类型: " + std::to_string(static_cast<int>(type)) + 
                ", 实际: " + std::to_string(static_cast<int>(token.type)));
    return false;
}

bool CHTLParser::matchToken(TokenType type) const {
    Token token = getCurrentToken();
    return token.type == type;
}

std::string CHTLParser::getTokenValue(const Token& token) const {
    return token.value;
}

void CHTLParser::setContext(std::shared_ptr<CHTLContext> context) {
    m_context = context;
}

std::shared_ptr<CHTLContext> CHTLParser::getContext() const {
    return m_context;
}

void CHTLParser::clearErrors() {
    m_errors.clear();
    m_warnings.clear();
}

void CHTLParser::reportError(const std::string& message) {
    m_errors.push_back(message);
    
    if (m_debugMode) {
        std::cout << "  ❌ 解析错误: " << message << std::endl;
    }
    
    ErrorReporter reporter;
    reporter.error("CHTLParser", message);
}

void CHTLParser::reportWarning(const std::string& message) {
    m_warnings.push_back(message);
    
    if (m_debugMode) {
        std::cout << "  ⚠️ 解析警告: " << message << std::endl;
    }
    
    ErrorReporter reporter;
    reporter.warning("CHTLParser", message);
}

bool CHTLParser::isKeyword(const std::string& word) const {
    std::vector<std::string> keywords = {
        "Template", "Custom", "Origin", "Import", "Namespace", "Configuration",
        "text", "style", "script", "inherit", "delete", "insert", "replace",
        "except", "use", "from", "as", "after", "before"
    };
    
    return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
}

}