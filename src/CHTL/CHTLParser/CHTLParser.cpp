#include "CHTLParser.h"
#include "../CHTLLexer/GlobalMap.h"
#include <sstream>

namespace CHTL {

// ========== CHTLParser 实现 ==========

CHTLParser::CHTLParser(const ParseOptions& options)
    : m_options(options)
    , m_errorReporter(&getGlobalErrorReporter())
    , m_currentIndex(0)
    , m_currentDepth(0) {
    
    m_stateMachine = std::make_unique<CHTLStateMachine>();
    m_context = std::make_unique<CHTLContext>();
}

ParseResult CHTLParser::parse(const TokenSequence& tokens) {
    reset();
    initializeParsing(tokens);
    
    try {
        parseMain();
        finalizeParsing();
        m_result.success = true;
    } catch (const std::exception& e) {
        reportError("Parse failed: " + std::string(e.what()));
        m_result.success = false;
    }
    
    return m_result;
}

ParseResult CHTLParser::parseFile(const std::string& filePath) {
    CHTLFileReader reader(filePath);
    if (!reader.open()) {
        reportError("Cannot open file: " + filePath);
        return m_result;
    }
    
    std::string content = reader.readAll();
    reader.close();
    
    return parseString(content);
}

ParseResult CHTLParser::parseString(const std::string& source) {
    Lexer lexer(source, m_stateMachine.get(), m_context.get(), m_errorReporter);
    TokenSequence tokens = lexer.tokenize();
    
    return parse(tokens);
}

void CHTLParser::setParseOptions(const ParseOptions& options) {
    m_options = options;
}

const ParseOptions& CHTLParser::getParseOptions() const {
    return m_options;
}

void CHTLParser::setErrorReporter(ErrorReporter* errorReporter) {
    m_errorReporter = errorReporter ? errorReporter : &getGlobalErrorReporter();
}

std::unordered_map<std::string, size_t> CHTLParser::getParseStatistics() const {
    return m_statistics;
}

void CHTLParser::reset() {
    m_tokens.clear();
    m_currentIndex = 0;
    m_currentDepth = 0;
    m_result = ParseResult();
    
    while (!m_nodeStack.empty()) {
        m_nodeStack.pop();
    }
    
    m_statistics.clear();
    
    if (m_stateMachine) {
        m_stateMachine->reset();
    }
    
    if (m_context) {
        m_context->reset();
    }
}

// ========== 私有方法实现 ==========

void CHTLParser::initializeParsing(const TokenSequence& tokens) {
    m_tokens = tokens;
    m_currentIndex = 0;
    m_currentDepth = 0;
    
    m_result.tokensProcessed = 0;
    m_result.nodesCreated = 0;
    
    // 创建根节点
    m_result.rootNode = createNode(CHTLNodeType::ROOT_NODE, "root");
    pushNode(m_result.rootNode);
    
    updateStatistics("tokens_total", m_tokens.size());
}

void CHTLParser::finalizeParsing() {
    m_result.tokensProcessed = m_currentIndex;
    updateStatistics("tokens_processed", m_result.tokensProcessed);
}

void CHTLParser::parseMain() {
    while (!isAtEnd()) {
        if (!checkLimits()) {
            reportError("Parse limits exceeded");
            break;
        }
        
        NodePtr node = parseTopLevel();
        if (node) {
            if (topNode()) {
                addChild(topNode(), node);
            }
            updateStatistics("top_level_nodes");
        } else {
            // 错误恢复
            if (m_options.autoRecovery) {
                synchronize();
            } else {
                break;
            }
        }
    }
}

NodePtr CHTLParser::parseTopLevel() {
    const Token& token = currentToken();
    
    switch (token.type) {
        case TokenType::HTML_ELEMENT:
            return parseElement();
            
        case TokenType::TEXT:
            return parseText();
            
        case TokenType::LINE_COMMENT:
        case TokenType::BLOCK_COMMENT:
        case TokenType::GENERATOR_COMMENT:
            if (m_options.enableComments) {
                return parseComment();
            }
            advance(); // 跳过注释
            return nullptr;
            
        case TokenType::TEMPLATE:
            if (m_options.enableTemplates) {
                return parseTemplate();
            }
            advance();
            return nullptr;
            
        case TokenType::CUSTOM:
            if (m_options.enableCustom) {
                return parseCustom();
            }
            advance();
            return nullptr;
            
        case TokenType::IMPORT:
            if (m_options.enableImports) {
                return parseImport();
            }
            advance();
            return nullptr;
            
        case TokenType::CONFIGURATION:
            if (m_options.enableConfig) {
                return parseConfiguration();
            }
            advance();
            return nullptr;
            
        case TokenType::NAMESPACE:
            return parseNamespace();
            
        case TokenType::USE:
            return parseUse();
            
        case TokenType::AT_STYLE:
        case TokenType::AT_ELEMENT:
        case TokenType::AT_VAR:
            // 模板使用
            return parseTemplateUsage();
            
        default:
            reportError("Unexpected token: " + getTokenString(token));
            advance();
            return nullptr;
    }
}

NodePtr CHTLParser::parseElement() {
    const Token& token = currentToken();
    if (token.type != TokenType::HTML_ELEMENT) {
        reportError("Expected HTML element");
        return nullptr;
    }
    
    auto element = std::make_shared<ElementNode>(token.value);
    element->setPosition(token.position);
    updateStatistics("elements");
    
    advance(); // 消费元素名
    
    // 解析属性（在大括号之前）
    parseAttributes(element.get());
    
    // 解析元素体
    if (match(TokenType::LEFT_BRACE)) {
        advance(); // 消费 {
        
        enterScope(CHTLNodeType::ELEMENT_NODE);
        parseBlockContent(element);
        exitScope();
        
        if (!consume(TokenType::RIGHT_BRACE, "Expected '}' after element body")) {
            return nullptr;
        }
    }
    
    return element;
}

NodePtr CHTLParser::parseText() {
    if (!consume(TokenType::TEXT, "Expected 'text' keyword")) {
        return nullptr;
    }
    
    auto textNode = std::make_shared<TextNode>();
    textNode->setPosition(currentToken().position);
    updateStatistics("text_nodes");
    
    if (!consume(TokenType::LEFT_BRACE, "Expected '{' after 'text'")) {
        return nullptr;
    }
    
    // 解析文本内容
    if (match({TokenType::DOUBLE_QUOTED_STRING, TokenType::SINGLE_QUOTED_STRING, TokenType::IDENTIFIER, TokenType::UNQUOTED_LITERAL})) {
        std::string content = parseLiteralValue();
        textNode->setContent(content);
        
        // 根据Token类型设置文本类型
        if (currentToken().type == TokenType::DOUBLE_QUOTED_STRING) {
            textNode->setDoubleQuoted(true);
        } else if (currentToken().type == TokenType::SINGLE_QUOTED_STRING) {
            textNode->setSingleQuoted(true);
        } else {
            textNode->setLiteral(true);
        }
        
        advance();
    }
    
    if (!consume(TokenType::RIGHT_BRACE, "Expected '}' after text content")) {
        return nullptr;
    }
    
    return textNode;
}

NodePtr CHTLParser::parseComment() {
    const Token& token = currentToken();
    CommentType commentType;
    
    switch (token.type) {
        case TokenType::LINE_COMMENT:
            commentType = CommentType::LINE_COMMENT;
            break;
        case TokenType::BLOCK_COMMENT:
            commentType = CommentType::BLOCK_COMMENT;
            break;
        case TokenType::GENERATOR_COMMENT:
            commentType = CommentType::GENERATOR_COMMENT;
            break;
        default:
            reportError("Expected comment token");
            return nullptr;
    }
    
    auto comment = std::make_shared<CommentNode>(commentType, token.value);
    comment->setPosition(token.position);
    updateStatistics("comments");
    
    advance();
    return comment;
}

NodePtr CHTLParser::parseStyleBlock() {
    if (!consume(TokenType::STYLE, "Expected 'style' keyword")) {
        return nullptr;
    }
    
    auto styleNode = std::make_shared<StyleNode>(StyleNodeType::LOCAL_STYLE);
    styleNode->setPosition(currentToken().position);
    updateStatistics("style_blocks");
    
    if (!consume(TokenType::LEFT_BRACE, "Expected '{' after 'style'")) {
        return nullptr;
    }
    
    enterScope(CHTLNodeType::STYLE_NODE);
    
    // 解析样式内容
    while (!match(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match(TokenType::CLASS_SELECTOR) || match(TokenType::ID_SELECTOR) || match(TokenType::AMPERSAND)) {
            // 选择器样式
            std::string selector = currentToken().value;
            advance();
            
            // 处理&后面的伪类或伪元素，如&:hover, &::before
            if (selector == "&" && match(TokenType::COLON)) {
                selector += currentToken().value; // 添加:hover等
                advance();
                
                // 检查是否为伪元素(::)
                if (match(TokenType::COLON)) {
                    selector += currentToken().value; // 添加第二个:
                    advance();
                }
                
                // 添加伪类/伪元素名称
                if (match(TokenType::IDENTIFIER)) {
                    selector += currentToken().value;
                    advance();
                }
            }
            
            styleNode->setSelector(selector);
            
            if (match(TokenType::LEFT_BRACE)) {
                advance();
                // 解析CSS属性
                while (!match(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                    if (match(TokenType::IDENTIFIER)) {
                        std::string property = currentToken().value;
                        advance();
                        
                        // CE对等式：支持冒号或等号
                        if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
                            advance(); // 消费冒号或等号
                            std::string value = parseLiteralValue();
                            styleNode->addCssProperty(property, value);
                            advance();
                            
                            match(TokenType::SEMICOLON); // 可选的分号
                            if (currentToken().type == TokenType::SEMICOLON) advance();
                        }
                    } else {
                        advance();
                    }
                }
                
                if (!consume(TokenType::RIGHT_BRACE, "Expected '}' after selector block")) {
                    break;
                }
            }
        } else if (match(TokenType::IDENTIFIER)) {
            // 内联样式属性
            std::string property = currentToken().value;
            advance();
            
            // CE对等式：支持冒号或等号
            if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
                advance(); // 消费冒号或等号
                std::string value = parseLiteralValue();
                styleNode->addCssProperty(property, value);
                advance();
                
                match(TokenType::SEMICOLON); // 可选的分号
                if (currentToken().type == TokenType::SEMICOLON) advance();
            }
        } else {
            advance();
        }
    }
    
    exitScope();
    
    if (!consume(TokenType::RIGHT_BRACE, "Expected '}' after style block")) {
        return nullptr;
    }
    
    return styleNode;
}

void CHTLParser::parseAttributes(ElementNode* element) {
    while (match(TokenType::IDENTIFIER) && 
           (peekToken().type == TokenType::COLON || peekToken().type == TokenType::EQUAL)) {
        if (!parseAttribute(element)) {
            break;
        }
    }
}

bool CHTLParser::parseAttribute(ElementNode* element) {
    if (!match(TokenType::IDENTIFIER)) {
        return false;
    }
    
    std::string attrName = currentToken().value;
    advance();
    
    // CE对等式：支持冒号或等号
    if (!match(TokenType::COLON) && !match(TokenType::EQUAL)) {
        reportError("Expected ':' or '=' after attribute name");
        return false;
    }
    advance(); // 消费冒号或等号
    
    std::string attrValue = parseLiteralValue();
    advance();
    
    // 消费可选的分号
    if (match(TokenType::SEMICOLON)) {
        advance();
    }
    
    element->setAttribute(attrName, attrValue);
    updateStatistics("attributes");
    
    return true;
}

// ========== Token操作方法 ==========

const Token& CHTLParser::currentToken() const {
    if (m_currentIndex >= m_tokens.size()) {
        static Token eofToken(TokenType::EOF_TOKEN, "", Position());
        return eofToken;
    }
    return m_tokens[m_currentIndex];
}

const Token& CHTLParser::nextToken() {
    if (m_currentIndex + 1 < m_tokens.size()) {
        return m_tokens[m_currentIndex + 1];
    }
    static Token eofToken(TokenType::EOF_TOKEN, "", Position());
    return eofToken;
}

const Token& CHTLParser::peekToken(size_t offset) const {
    size_t index = m_currentIndex + offset;
    if (index >= m_tokens.size()) {
        static Token eofToken(TokenType::EOF_TOKEN, "", Position());
        return eofToken;
    }
    return m_tokens[index];
}

bool CHTLParser::isAtEnd() const {
    return m_currentIndex >= m_tokens.size() || currentToken().type == TokenType::EOF_TOKEN;
}

void CHTLParser::advance() {
    if (!isAtEnd()) {
        m_currentIndex++;
        m_result.tokensProcessed++;
    }
}

bool CHTLParser::expect(TokenType type) {
    return currentToken().type == type;
}

bool CHTLParser::match(TokenType type) {
    return currentToken().type == type;
}

bool CHTLParser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (match(type)) {
            return true;
        }
    }
    return false;
}

bool CHTLParser::consume(TokenType type, const std::string& errorMessage) {
    if (match(type)) {
        advance();
        return true;
    }
    
    if (!errorMessage.empty()) {
        reportError(errorMessage);
    }
    return false;
}

// ========== 辅助方法 ==========

NodePtr CHTLParser::createNode(CHTLNodeType type, const std::string& name) {
    auto node = NodeFactory::createNode(type, name);
    m_result.nodesCreated++;
    updateStatistics("nodes_created");
    return node;
}

void CHTLParser::addChild(NodePtr parent, NodePtr child) {
    if (parent && child) {
        parent->addChild(child);
    }
}

void CHTLParser::pushNode(NodePtr node) {
    m_nodeStack.push(node);
}

NodePtr CHTLParser::popNode() {
    if (!m_nodeStack.empty()) {
        NodePtr node = m_nodeStack.top();
        m_nodeStack.pop();
        return node;
    }
    return nullptr;
}

NodePtr CHTLParser::topNode() const {
    return m_nodeStack.empty() ? nullptr : m_nodeStack.top();
}

void CHTLParser::updateStatistics(const std::string& key, size_t increment) {
    m_statistics[key] += increment;
}

bool CHTLParser::checkDepthLimit() {
    return m_currentDepth < m_options.maxDepth;
}

void CHTLParser::enterScope(CHTLNodeType nodeType) {
    m_currentDepth++;
    // 状态机和上下文管理
    if (m_stateMachine) {
        // 简化状态转换
    }
}

void CHTLParser::exitScope() {
    if (m_currentDepth > 0) {
        m_currentDepth--;
    }
}

void CHTLParser::parseBlockContent(NodePtr parent) {
    while (!match(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        NodePtr child = nullptr;
        
        if (match(TokenType::HTML_ELEMENT)) {
            child = parseElement();
        } else if (match(TokenType::TEXT)) {
            child = parseText();
        } else if (match(TokenType::STYLE)) {
            child = parseStyleBlock();
        } else if (match({TokenType::LINE_COMMENT, TokenType::BLOCK_COMMENT, TokenType::GENERATOR_COMMENT})) {
            if (m_options.enableComments) {
                child = parseComment();
            } else {
                advance();
            }
        } else {
            advance(); // 跳过未知Token
        }
        
        if (child && parent) {
            addChild(parent, child);
        }
    }
}

std::string CHTLParser::parseLiteralValue() {
    const Token& token = currentToken();
    
    switch (token.type) {
        case TokenType::DOUBLE_QUOTED_STRING:
        case TokenType::SINGLE_QUOTED_STRING:
        case TokenType::IDENTIFIER:
        case TokenType::UNQUOTED_LITERAL:
            return token.value;
        default:
            // 检查是否为变量引用模式：VarGroupName(varName)
            if (token.type == TokenType::IDENTIFIER) {
                std::string value = token.value;
                // 向前看，检查是否跟着括号
                Token nextToken = peekToken();
                if (nextToken.type == TokenType::LEFT_PAREN) {
                    // 这可能是变量引用，保持原样返回，让变量替换系统处理
                    return value;
                }
            }
            return "";
    }
}

void CHTLParser::reportError(const std::string& message) {
    if (m_errorReporter) {
        Position pos = isAtEnd() ? Position() : currentToken().position;
        ErrorPosition errorPos("", pos.line, pos.column, pos.offset);
        m_errorReporter->error(ErrorType::SYNTAX_ERROR, message, errorPos);
    }
}

void CHTLParser::reportError(const std::string& message, const Position& position) {
    if (m_errorReporter) {
        ErrorPosition errorPos("", position.line, position.column, position.offset);
        m_errorReporter->error(ErrorType::SYNTAX_ERROR, message, errorPos);
    }
}

void CHTLParser::reportWarning(const std::string& message) {
    if (m_errorReporter) {
        Position pos = isAtEnd() ? Position() : currentToken().position;
        ErrorPosition errorPos("", pos.line, pos.column, pos.offset);
        m_errorReporter->warning(message, errorPos);
    }
}

void CHTLParser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (match({TokenType::HTML_ELEMENT, TokenType::TEXT, TokenType::STYLE, 
                  TokenType::TEMPLATE, TokenType::CUSTOM, TokenType::IMPORT})) {
            return;
        }
        advance();
    }
}

bool CHTLParser::checkLimits() {
    if (m_result.tokensProcessed >= m_options.maxTokens) {
        return false;
    }
    
    if (m_currentDepth >= m_options.maxDepth) {
        return false;
    }
    
    return true;
}

std::string CHTLParser::getTokenString(const Token& token) const {
    return token.value.empty() ? "EOF" : token.value;
}

// ========== 模板和自定义解析实现 ==========

NodePtr CHTLParser::parseTemplate() {
    if (!consume(TokenType::TEMPLATE, "Expected '[Template]'")) {
        return nullptr;
    }
    
    // 解析模板类型 (@Style, @Element, @Var)
    if (!match({TokenType::AT_STYLE, TokenType::AT_ELEMENT, TokenType::AT_VAR})) {
        reportError("Expected template type (@Style, @Element, or @Var)");
        return nullptr;
    }
    
    TokenType templateType = currentToken().type;
    advance();
    
    // 解析模板名称
    if (!match(TokenType::IDENTIFIER)) {
        reportError("Expected template name");
        return nullptr;
    }
    
    std::string templateName = currentToken().value;
    advance();
    
    // 根据类型解析不同的模板
    switch (templateType) {
        case TokenType::AT_STYLE:
            return parseStyleTemplate(templateName, false);
        case TokenType::AT_ELEMENT:
            return parseElementTemplate(templateName, false);
        case TokenType::AT_VAR:
            return parseVarTemplate(templateName, false);
        default:
            reportError("Unsupported template type");
            return nullptr;
    }
}

NodePtr CHTLParser::parseCustom() {
    if (!consume(TokenType::CUSTOM, "Expected '[Custom]'")) {
        return nullptr;
    }
    
    // 解析自定义类型 (@Style, @Element, @Var)
    if (!match({TokenType::AT_STYLE, TokenType::AT_ELEMENT, TokenType::AT_VAR})) {
        reportError("Expected custom type (@Style, @Element, or @Var)");
        return nullptr;
    }
    
    TokenType customType = currentToken().type;
    advance();
    
    // 解析自定义名称
    if (!match(TokenType::IDENTIFIER)) {
        reportError("Expected custom name");
        return nullptr;
    }
    
    std::string customName = currentToken().value;
    advance();
    
    // 根据类型解析不同的自定义
    switch (customType) {
        case TokenType::AT_STYLE:
            return parseStyleTemplate(customName, true);
        case TokenType::AT_ELEMENT:
            return parseElementTemplate(customName, true);
        case TokenType::AT_VAR:
            return parseVarTemplate(customName, true);
        default:
            reportError("Unsupported custom type");
            return nullptr;
    }
}
// ========== 完整的模板解析实现 ==========

NodePtr CHTLParser::parseStyleTemplate(const std::string& name, bool isCustom) {
    auto styleTemplate = std::make_shared<AdvancedStyleTemplateNode>(name);
    updateStatistics(isCustom ? "custom_style_templates" : "style_templates");
    
    if (!consume(TokenType::LEFT_BRACE, "Expected '{' after template name")) {
        return nullptr;
    }
    
    enterScope(CHTLNodeType::TEMPLATE_STYLE_NODE);
    
    while (!match(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match(TokenType::INHERIT)) {
            // 显式继承: inherit @Style TemplateName;
            advance(); // consume 'inherit'
            
            if (match(TokenType::AT_STYLE)) {
                advance();
                if (match(TokenType::IDENTIFIER)) {
                    std::string inheritedTemplate = currentToken().value;
                    styleTemplate->addInheritance(inheritedTemplate, InheritanceType::EXPLICIT);
                    advance();
                    
                    if (match(TokenType::SEMICOLON)) advance();
                }
            }
        }
        else if (match(TokenType::AT_STYLE)) {
            // 组合式继承: @Style TemplateName;
            advance();
            if (match(TokenType::IDENTIFIER)) {
                std::string inheritedTemplate = currentToken().value;
                styleTemplate->addInheritance(inheritedTemplate, InheritanceType::COMPOSITION);
                advance();
                
                // 解析可能的参数
                if (match(TokenType::LEFT_BRACE)) {
                    auto params = parseTemplateParameters();
                    // 将参数添加到继承信息中
                }
                
                if (match(TokenType::SEMICOLON)) advance();
            }
        }
        else if (match(TokenType::DELETE)) {
            // 删除操作: delete property1, property2;
            advance(); // consume 'delete'
            
            do {
                if (match(TokenType::IDENTIFIER)) {
                    styleTemplate->deleteProperty(currentToken().value);
                    advance();
                } else if (match(TokenType::AT_STYLE)) {
                    advance();
                    if (match(TokenType::IDENTIFIER)) {
                        styleTemplate->deleteInheritance(currentToken().value);
                        advance();
                    }
                }
                
                if (match(TokenType::COMMA)) advance();
                else break;
            } while (!isAtEnd());
            
            if (match(TokenType::SEMICOLON)) advance();
        }
        else if (match(TokenType::IDENTIFIER)) {
            // CSS属性
            std::string property = currentToken().value;
            advance();
            
            if (match({TokenType::COLON, TokenType::EQUAL})) {
                // 有值的属性
                advance();
                std::string value = parseLiteralValue();
                styleTemplate->setParameter(property, value);
                advance();
            } else if (match(TokenType::COMMA) || match(TokenType::SEMICOLON)) {
                // 无值属性（自定义样式组特有）
                if (isCustom) {
                    styleTemplate->addPropertyWithoutValue(property);
                }
            } else {
                // 单独的无值属性
                if (isCustom) {
                    styleTemplate->addPropertyWithoutValue(property);
                }
            }
            
            if (match(TokenType::SEMICOLON)) advance();
        }
        else {
            advance(); // 跳过未知token
        }
    }
    
    exitScope();
    
    if (!consume(TokenType::RIGHT_BRACE, "Expected '}' after template body")) {
        return nullptr;
    }
    
    return styleTemplate;
}

NodePtr CHTLParser::parseElementTemplate(const std::string& name, bool isCustom) {
    auto elementTemplate = std::make_shared<AdvancedElementTemplateNode>(name);
    updateStatistics(isCustom ? "custom_element_templates" : "element_templates");
    
    if (!consume(TokenType::LEFT_BRACE, "Expected '{' after template name")) {
        return nullptr;
    }
    
    enterScope(CHTLNodeType::TEMPLATE_ELEMENT_NODE);
    
    while (!match(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match(TokenType::INHERIT)) {
            // 显式继承
            advance();
            if (match(TokenType::AT_ELEMENT)) {
                advance();
                if (match(TokenType::IDENTIFIER)) {
                    elementTemplate->addInheritance(currentToken().value, InheritanceType::EXPLICIT);
                    advance();
                    if (match(TokenType::SEMICOLON)) advance();
                }
            }
        }
        else if (match(TokenType::AT_ELEMENT)) {
            // 组合式继承
            advance();
            if (match(TokenType::IDENTIFIER)) {
                elementTemplate->addInheritance(currentToken().value, InheritanceType::COMPOSITION);
                advance();
                if (match(TokenType::SEMICOLON)) advance();
            }
        }
        else if (match(TokenType::DELETE)) {
            // 删除元素
            advance();
            parseDeleteElementOperation(elementTemplate.get());
        }
        else if (match(TokenType::INSERT)) {
            // 插入元素
            advance();
            parseInsertElementOperation(elementTemplate.get());
        }
        else {
            // 普通元素内容
            NodePtr child = parseTopLevel();
            if (child) {
                elementTemplate->addChild(child);
            }
        }
    }
    
    exitScope();
    
    if (!consume(TokenType::RIGHT_BRACE, "Expected '}' after template body")) {
        return nullptr;
    }
    
    return elementTemplate;
}

NodePtr CHTLParser::parseVarTemplate(const std::string& name, bool isCustom) {
    auto varTemplate = std::make_shared<AdvancedVarTemplateNode>(name);
    updateStatistics(isCustom ? "custom_var_templates" : "var_templates");
    
    if (!consume(TokenType::LEFT_BRACE, "Expected '{' after template name")) {
        return nullptr;
    }
    
    enterScope(CHTLNodeType::TEMPLATE_VAR_NODE);
    
    while (!match(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match(TokenType::INHERIT)) {
            // 显式继承
            advance();
            if (match(TokenType::AT_VAR)) {
                advance();
                if (match(TokenType::IDENTIFIER)) {
                    varTemplate->addInheritance(currentToken().value, InheritanceType::EXPLICIT);
                    advance();
                    if (match(TokenType::SEMICOLON)) advance();
                }
            }
        }
        else if (match(TokenType::AT_VAR)) {
            // 组合式继承
            advance();
            if (match(TokenType::IDENTIFIER)) {
                varTemplate->addInheritance(currentToken().value, InheritanceType::COMPOSITION);
                advance();
                if (match(TokenType::SEMICOLON)) advance();
            }
        }
        else if (match(TokenType::IDENTIFIER)) {
            // 变量定义
            std::string varName = currentToken().value;
            advance();
            
            if (match({TokenType::COLON, TokenType::EQUAL})) {
                advance();
                std::string varValue = parseLiteralValue();
                varTemplate->setVariable(varName, varValue);
                advance();
            }
            
            if (match(TokenType::SEMICOLON)) advance();
        }
        else {
            advance(); // 跳过未知token
        }
    }
    
    exitScope();
    
    if (!consume(TokenType::RIGHT_BRACE, "Expected '}' after template body")) {
        return nullptr;
    }
    
    return varTemplate;
}

NodePtr CHTLParser::parseTemplateUsage() {
    // 解析模板使用: @Style TemplateName; 或 @Style TemplateName { params }
    if (!match({TokenType::AT_STYLE, TokenType::AT_ELEMENT, TokenType::AT_VAR})) {
        return nullptr;
    }
    
    std::string templateType;
    switch (currentToken().type) {
        case TokenType::AT_STYLE: templateType = "@Style"; break;
        case TokenType::AT_ELEMENT: templateType = "@Element"; break;
        case TokenType::AT_VAR: templateType = "@Var"; break;
        default: return nullptr;
    }
    advance();
    
    if (!match(TokenType::IDENTIFIER)) {
        reportError("Expected template name");
        return nullptr;
    }
    
    std::string templateName = currentToken().value;
    advance();
    
    auto usage = std::make_shared<TemplateUsageNode>(templateType, templateName);
    
    // 解析参数（如果有）
    if (match(TokenType::LEFT_BRACE)) {
        auto params = parseTemplateParameters();
        for (const auto& [name, value] : params) {
            usage->setParameter(name, value);
        }
        
        // 解析特例化操作（如果有）
        auto specializations = parseSpecializations();
        for (const auto& spec : specializations) {
            usage->addSpecialization(spec);
        }
    }
    
    if (match(TokenType::SEMICOLON)) advance();
    
    updateStatistics("template_usages");
    return usage;
}

std::unordered_map<std::string, std::string> CHTLParser::parseTemplateParameters() {
    std::unordered_map<std::string, std::string> params;
    
    if (!consume(TokenType::LEFT_BRACE, "Expected '{'")) {
        return params;
    }
    
    while (!match(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match(TokenType::IDENTIFIER)) {
            std::string paramName = currentToken().value;
            advance();
            
            if (match({TokenType::COLON, TokenType::EQUAL})) {
                advance();
                std::string paramValue = parseComplexValue(); // 支持变量引用的值解析
                params[paramName] = paramValue;
                advance();
            }
            
            if (match(TokenType::SEMICOLON)) advance();
            if (match(TokenType::COMMA)) advance();
        } else {
            advance();
        }
    }
    
    if (!consume(TokenType::RIGHT_BRACE, "Expected '}'")) {
        return params;
    }
    
    return params;
}

std::string CHTLParser::parseComplexValue() {
    // 解析可能包含变量引用的复杂值
    std::string result;
    
    if (match(TokenType::IDENTIFIER)) {
        std::string value = currentToken().value;
        
        // 检查是否为变量引用
        if (peekToken().type == TokenType::LEFT_PAREN) {
            // 这是变量引用 VarGroup(varName) 或 VarGroup(varName = value)
            result = value;
            advance(); // consume identifier
            
            if (match(TokenType::LEFT_PAREN)) {
                result += currentToken().value; // 加上 '('
                advance();
                
                // 解析变量名和可能的覆盖值
                while (!match(TokenType::RIGHT_PAREN) && !isAtEnd()) {
                    result += currentToken().value;
                    advance();
                }
                
                if (match(TokenType::RIGHT_PAREN)) {
                    result += currentToken().value; // 加上 ')'
                }
            }
        } else {
            result = value;
        }
    } else {
        result = parseLiteralValue();
    }
    
    return result;
}

void CHTLParser::parseDeleteElementOperation(AdvancedElementTemplateNode* elementTemplate) {
    if (match(TokenType::IDENTIFIER)) {
        std::string target = currentToken().value;
        advance();
        
        if (match(TokenType::LEFT_BRACKET)) {
            // delete tagName[index];
            advance(); // consume '['
            if (match(TokenType::IDENTIFIER)) {
                int index = std::stoi(currentToken().value);
                elementTemplate->deleteElement(index);
                advance();
            }
            if (!consume(TokenType::RIGHT_BRACKET, "Expected ']'")) return;
        } else {
            // delete tagName;
            elementTemplate->deleteElement(target);
        }
    } else if (match(TokenType::AT_ELEMENT)) {
        // delete @Element TemplateName;
        advance();
        if (match(TokenType::IDENTIFIER)) {
            elementTemplate->deleteInheritance(currentToken().value);
            advance();
        }
    }
    
    if (match(TokenType::SEMICOLON)) advance();
}

void CHTLParser::parseInsertElementOperation(AdvancedElementTemplateNode* elementTemplate) {
    InsertPosition position = InsertPosition::AFTER;
    
    // 解析插入位置
    if (match(TokenType::AFTER)) {
        position = InsertPosition::AFTER;
        advance();
    } else if (match(TokenType::BEFORE)) {
        position = InsertPosition::BEFORE;
        advance();
    } else if (match(TokenType::REPLACE)) {
        position = InsertPosition::REPLACE;
        advance();
    } else if (match(TokenType::AT_TOP)) {
        position = InsertPosition::AT_TOP;
        advance();
    } else if (match(TokenType::AT_BOTTOM)) {
        position = InsertPosition::AT_BOTTOM;
        advance();
    }
    
    // 解析目标选择器或索引
    std::string selector;
    int index = -1;
    
    if (match(TokenType::IDENTIFIER)) {
        selector = currentToken().value;
        advance();
        
        if (match(TokenType::LEFT_BRACKET)) {
            advance();
            if (match(TokenType::IDENTIFIER)) {
                index = std::stoi(currentToken().value);
                selector += "[" + currentToken().value + "]";
                advance();
            }
            if (!consume(TokenType::RIGHT_BRACKET, "Expected ']'")) return;
        }
    }
    
    // 解析插入内容
    std::vector<NodePtr> insertContent;
    if (match(TokenType::LEFT_BRACE)) {
        advance();
        
        while (!match(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            NodePtr child = parseTopLevel();
            if (child) {
                insertContent.push_back(child);
            }
        }
        
        if (!consume(TokenType::RIGHT_BRACE, "Expected '}' after insert content")) {
            return;
        }
    }
    
    // 应用插入操作
    if (index >= 0) {
        elementTemplate->insertElement(position, index, insertContent);
    } else {
        elementTemplate->insertElement(position, selector, insertContent);
    }
}

std::vector<SpecializationInfo> CHTLParser::parseSpecializations() {
    std::vector<SpecializationInfo> specializations;
    
    while (!match(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match(TokenType::DELETE)) {
            specializations.push_back(parseDeleteOperation());
        } else if (match(TokenType::INSERT)) {
            specializations.push_back(parseInsertOperation());
        } else {
            break;
        }
    }
    
    return specializations;
}

SpecializationInfo CHTLParser::parseDeleteOperation() {
    SpecializationInfo spec;
    spec.type = SpecializationType::DELETE_PROPERTY;
    
    advance(); // consume 'delete'
    
    if (match(TokenType::IDENTIFIER)) {
        spec.target = currentToken().value;
        advance();
    } else if (match(TokenType::AT_STYLE) || match(TokenType::AT_ELEMENT) || match(TokenType::AT_VAR)) {
        // delete @Style TemplateName;
        spec.type = SpecializationType::DELETE_INHERITANCE;
        advance();
        if (match(TokenType::IDENTIFIER)) {
            spec.target = currentToken().value;
            advance();
        }
    }
    
    if (match(TokenType::SEMICOLON)) advance();
    
    return spec;
}

SpecializationInfo CHTLParser::parseInsertOperation() {
    SpecializationInfo spec;
    spec.type = SpecializationType::INSERT_ELEMENT;
    
    advance(); // consume 'insert'
    
    // 解析位置
    if (match(TokenType::AFTER)) {
        spec.position = InsertPosition::AFTER;
        advance();
    } else if (match(TokenType::BEFORE)) {
        spec.position = InsertPosition::BEFORE;
        advance();
    } else if (match(TokenType::REPLACE)) {
        spec.position = InsertPosition::REPLACE;
        advance();
    }
    
    // 解析目标
    if (match(TokenType::IDENTIFIER)) {
        spec.target = currentToken().value;
        advance();
        
        // 解析索引
        if (match(TokenType::LEFT_BRACKET)) {
            advance();
            if (match(TokenType::IDENTIFIER)) {
                spec.index = std::stoi(currentToken().value);
                advance();
            }
            if (!consume(TokenType::RIGHT_BRACKET, "Expected ']'")) {
                return spec;
            }
        }
    }
    
    // 解析插入内容
    if (match(TokenType::LEFT_BRACE)) {
        advance();
        while (!match(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            NodePtr content = parseTopLevel();
            if (content) {
                spec.content.push_back(content);
            }
        }
        if (!consume(TokenType::RIGHT_BRACE, "Expected '}'")) {
            return spec;
        }
    }
    
    return spec;
}

NodePtr CHTLParser::parseVariableReference() { advance(); return nullptr; }
NodePtr CHTLParser::parseInheritStatement() { advance(); return nullptr; }
SpecializationInfo CHTLParser::parseIndexAccess() { return SpecializationInfo(); }

NodePtr CHTLParser::parseImport() { advance(); return nullptr; }
NodePtr CHTLParser::parseConfiguration() { advance(); return nullptr; }
NodePtr CHTLParser::parseNamespace() { advance(); return nullptr; }
NodePtr CHTLParser::parseUse() { advance(); return nullptr; }

// ========== CHTLParserFactory 实现 ==========

std::unique_ptr<CHTLParser> CHTLParserFactory::createStandardParser() {
    ParseOptions options;
    return std::make_unique<CHTLParser>(options);
}

std::unique_ptr<CHTLParser> CHTLParserFactory::createStrictParser() {
    ParseOptions options;
    options.strictMode = true;
    options.autoRecovery = false;
    return std::make_unique<CHTLParser>(options);
}

std::unique_ptr<CHTLParser> CHTLParserFactory::createTolerantParser() {
    ParseOptions options;
    options.strictMode = false;
    options.autoRecovery = true;
    return std::make_unique<CHTLParser>(options);
}

std::unique_ptr<CHTLParser> CHTLParserFactory::createCustomParser(const ParseOptions& options) {
    return std::make_unique<CHTLParser>(options);
}

} // namespace CHTL