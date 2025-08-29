#include "Parser.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/ConfigNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/OperatorNode.h"
#include <sstream>

namespace CHTL {

Parser::Parser(std::shared_ptr<Lexer> lexer, std::shared_ptr<CompileContext> context,
               const ParserConfig& config)
    : lexer_(lexer), context_(context), config_(config) {
    // 读取第一个Token
    advance();
}

std::shared_ptr<ProgramNode> Parser::parse() {
    TokenLocation loc(1, 1, 0, 0);
    auto program = std::make_shared<ProgramNode>(context_->getSourceFile(), loc);
    
    // 解析use语句（如果有）
    if (check(TokenType::KEYWORD_USE)) {
        program->addTopLevelNode(parseUseStatement());
    }
    
    // 解析顶层节点
    while (!isAtEnd()) {
        try {
            auto node = parseTopLevel();
            if (node) {
                program->addTopLevelNode(node);
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
        // 在分号后同步
        if (previous_->getType() == TokenType::SEMICOLON) return;
        
        // 在关键字前同步
        switch (current_->getType()) {
            case TokenType::KEYWORD_TEXT:
            case TokenType::KEYWORD_STYLE:
            case TokenType::KEYWORD_SCRIPT:
            case TokenType::KEYWORD_TEMPLATE:
            case TokenType::KEYWORD_CUSTOM:
            case TokenType::KEYWORD_ORIGIN:
            case TokenType::KEYWORD_IMPORT:
            case TokenType::KEYWORD_CONFIGURATION:
            case TokenType::KEYWORD_NAMESPACE:
            case TokenType::HTML_TAG:
                return;
            default:
                break;
        }
        
        advance();
    }
}

std::shared_ptr<ASTNode> Parser::parseTopLevel() {
    // 跳过注释
    if (match({TokenType::SINGLE_LINE_COMMENT, TokenType::MULTI_LINE_COMMENT})) {
        // 暂时跳过注释
        return nullptr;
    }
    
    // 生成器注释
    if (check(TokenType::GENERATOR_COMMENT)) {
        auto token = current_;
        advance();
        return std::make_shared<CommentNode>(CommentType::GENERATOR, 
                                            token->getLexeme(), 
                                            token->getLocation());
    }
    
    // [Template]
    if (check(TokenType::KEYWORD_TEMPLATE)) {
        return parseTemplate();
    }
    
    // [Custom]
    if (check(TokenType::KEYWORD_CUSTOM)) {
        return parseCustom();
    }
    
    // [Origin]
    if (check(TokenType::KEYWORD_ORIGIN)) {
        return parseOrigin();
    }
    
    // [Import]
    if (check(TokenType::KEYWORD_IMPORT)) {
        return parseImport();
    }
    
    // [Configuration]
    if (check(TokenType::KEYWORD_CONFIGURATION)) {
        return parseConfiguration();
    }
    
    // [Namespace]
    if (check(TokenType::KEYWORD_NAMESPACE)) {
        return parseNamespace();
    }
    
    // [Info]
    if (check(TokenType::KEYWORD_INFO)) {
        return parseInfo();
    }
    
    // [Export]
    if (check(TokenType::KEYWORD_EXPORT)) {
        return parseExport();
    }
    
    // HTML元素
    if (check(TokenType::HTML_TAG) || check(TokenType::IDENTIFIER)) {
        std::string name = current_->getLexeme();
        if (isHtmlTag(name)) {
            return parseElement();
        }
    }
    
    // 全局style块
    if (check(TokenType::KEYWORD_STYLE)) {
        return parseStyleBlock();
    }
    
    // 全局script块
    if (check(TokenType::KEYWORD_SCRIPT)) {
        return parseScriptBlock();
    }
    
    error(*current_, "Unexpected token at top level");
    advance();
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::parseUseStatement() {
    auto location = current_->getLocation();
    consume(TokenType::KEYWORD_USE, "Expected 'use'");
    
    std::string target;
    if (check(TokenType::KEYWORD_HTML5)) {
        target = "html5";
        advance();
    } else if (match(TokenType::TYPE_CONFIG)) {
        target = parseIdentifier();
    } else if (match(TokenType::KEYWORD_CONFIGURATION)) {
        consume(TokenType::TYPE_CONFIG, "Expected '@Config' after '[Configuration]'");
        target = parseIdentifier();
    } else {
        error("Expected 'html5' or config name after 'use'");
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after use statement");
    
    return std::make_shared<UseNode>(target, location);
}

std::shared_ptr<ElementNode> Parser::parseElement() {
    auto location = current_->getLocation();
    std::string tagName = parseIdentifier();
    
    auto element = std::make_shared<ElementNode>(tagName, location);
    
    // 检查索引访问 div[0]
    if (match(TokenType::LEFT_BRACKET)) {
        if (check(TokenType::NUMBER_LITERAL)) {
            auto indexToken = current_;
            advance();
            try {
                if (std::holds_alternative<int64_t>(indexToken->getValue())) {
                    element->setIndex(std::get<int64_t>(indexToken->getValue()));
                } else if (std::holds_alternative<double>(indexToken->getValue())) {
                    element->setIndex(static_cast<size_t>(std::get<double>(indexToken->getValue())));
                }
            } catch (const std::bad_variant_access& e) {
                error("Invalid index value");
            }
        }
        consume(TokenType::RIGHT_BRACKET, "Expected ']' after index");
    }
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after element name");
    
    // 进入元素状态
    enterState(StateType::IN_ELEMENT);
    
    // 解析元素内容
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto content = parseElementContent();
        if (content) {
            if (content->getType() == NodeType::ATTRIBUTE) {
                auto attr = std::static_pointer_cast<AttributeNode>(content);
                element->addAttribute(attr->getName(), attr->getValue());
            } else {
                element->addChild(content);
            }
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after element content");
    
    // 退出元素状态
    exitState();
    
    return element;
}

std::shared_ptr<ASTNode> Parser::parseElementContent() {
    // 属性
    if (check(TokenType::IDENTIFIER)) {
        auto next = lexer_->peekToken();
        if (next && (next->getType() == TokenType::COLON || 
                    next->getType() == TokenType::EQUAL)) {
            return parseAttribute();
        }
    }
    
    // text块
    if (check(TokenType::KEYWORD_TEXT)) {
        return parseTextBlock();
    }
    
    // style块
    if (check(TokenType::KEYWORD_STYLE)) {
        return parseStyleBlock();
    }
    
    // script块
    if (check(TokenType::KEYWORD_SCRIPT)) {
        return parseScriptBlock();
    }
    
    // except约束
    if (check(TokenType::KEYWORD_EXCEPT)) {
        return parseExceptConstraint();
    }
    
    // 嵌套元素
    if (check(TokenType::HTML_TAG) || check(TokenType::IDENTIFIER)) {
        std::string name = current_->getLexeme();
        if (isHtmlTag(name)) {
            return parseElement();
        }
    }
    
    // 模板/自定义使用
    if (match({TokenType::TYPE_STYLE, TokenType::TYPE_ELEMENT, TokenType::TYPE_VAR})) {
        previous_->getType();  // 保存类型
        return parseTemplateUse();
    }
    
    // [Origin]嵌入
    if (check(TokenType::KEYWORD_ORIGIN)) {
        return parseOrigin();
    }
    
    error(*current_, "Unexpected token in element");
    advance();
    return nullptr;
}

std::shared_ptr<TextNode> Parser::parseTextBlock() {
    auto location = current_->getLocation();
    consume(TokenType::KEYWORD_TEXT, "Expected 'text'");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'text'");
    
    enterState(StateType::IN_TEXT_BLOCK);
    
    std::string content;
    if (check(TokenType::STRING_LITERAL)) {
        content = parseString();
    } else if (check(TokenType::UNQUOTED_LITERAL) || check(TokenType::IDENTIFIER)) {
        content = parseUnquotedLiteral();
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after text content");
    
    exitState();
    
    return std::make_shared<TextNode>(content, location);
}

std::shared_ptr<AttributeNode> Parser::parseAttribute() {
    auto location = current_->getLocation();
    std::string name = parseIdentifier();
    
    // CE对等式：: 和 = 等价
    if (!match({TokenType::COLON, TokenType::EQUAL})) {
        error("Expected ':' or '=' after attribute name");
    }
    
    std::string value = parseAttributeValue();
    
    consume(TokenType::SEMICOLON, "Expected ';' after attribute value");
    
    return std::make_shared<AttributeNode>(name, value, location);
}

std::string Parser::parseAttributeValue() {
    if (check(TokenType::STRING_LITERAL)) {
        return parseString();
    } else if (check(TokenType::UNQUOTED_LITERAL) || check(TokenType::IDENTIFIER)) {
        return parseUnquotedLiteral();
    } else if (check(TokenType::NUMBER_LITERAL)) {
        auto token = current_;
        advance();
        if (std::holds_alternative<int64_t>(token->getValue())) {
            return std::to_string(std::get<int64_t>(token->getValue()));
        } else {
            return std::to_string(std::get<double>(token->getValue()));
        }
    }
    
    error("Expected attribute value");
    return "";
}

std::shared_ptr<ASTNode> Parser::parseStyleBlock() {
    auto location = current_->getLocation();
    consume(TokenType::KEYWORD_STYLE, "Expected 'style'");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'style'");
    
    StyleBlockType blockType = isInState(StateType::IN_ELEMENT) ? 
                              StyleBlockType::LOCAL : StyleBlockType::GLOBAL;
    
    auto styleNode = std::make_shared<StyleNode>(blockType, location);
    
    enterState(StateType::IN_STYLE_BLOCK);
    context_->setInLocalStyleBlock(blockType == StyleBlockType::LOCAL);
    
    // 解析样式内容
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto rule = parseStyleContent();
        if (rule) {
            styleNode->addRule(rule);
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after style content");
    
    context_->setInLocalStyleBlock(false);
    exitState();
    
    return styleNode;
}

std::shared_ptr<ASTNode> Parser::parseScriptBlock() {
    auto location = current_->getLocation();
    consume(TokenType::KEYWORD_SCRIPT, "Expected 'script'");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'script'");
    
    ScriptBlockType blockType = isInState(StateType::IN_ELEMENT) ? 
                               ScriptBlockType::LOCAL : ScriptBlockType::GLOBAL;
    
    auto scriptNode = std::make_shared<ScriptNode>(blockType, location);
    
    enterState(StateType::IN_SCRIPT_BLOCK);
    context_->setInLocalScriptBlock(blockType == ScriptBlockType::LOCAL);
    
    // 收集脚本内容直到 }
    std::string content;
    int braceDepth = 0;
    
    while (!isAtEnd()) {
        if (check(TokenType::LEFT_BRACE)) {
            braceDepth++;
        } else if (check(TokenType::RIGHT_BRACE)) {
            if (braceDepth == 0) break;
            braceDepth--;
        }
        
        content += current_->getLexeme();
        if (check(TokenType::NEWLINE)) {
            content += "\n";
        } else {
            content += " ";
        }
        advance();
    }
    
    scriptNode->setContent(content);
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after script content");
    
    context_->setInLocalScriptBlock(false);
    exitState();
    
    return scriptNode;
}

std::string Parser::parseIdentifier() {
    if (!check(TokenType::IDENTIFIER) && !check(TokenType::HTML_TAG)) {
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
    
    std::string str;
    try {
        if (std::holds_alternative<std::string>(current_->getValue())) {
            str = std::get<std::string>(current_->getValue());
        } else {
            // If it's not a string, use the lexeme
            str = current_->getLexeme();
        }
    } catch (const std::bad_variant_access& e) {
        // Fallback to lexeme
        str = current_->getLexeme();
    }
    advance();
    return str;
}

std::string Parser::parseUnquotedLiteral() {
    std::string literal;
    
    while (!isAtEnd() && 
           (check(TokenType::UNQUOTED_LITERAL) || 
            check(TokenType::IDENTIFIER) ||
            check(TokenType::NUMBER_LITERAL))) {
        literal += current_->getLexeme();
        advance();
        
        // 检查是否到达结束符号
        if (check(TokenType::SEMICOLON) || check(TokenType::RIGHT_BRACE)) {
            break;
        }
    }
    
    return literal;
}

bool Parser::isHtmlTag(const std::string& name) {
    // 这里应该检查是否为有效的HTML标签
    // 暂时简化处理，假设所有小写标识符都是HTML标签
    return !name.empty() && std::islower(name[0]);
}

void Parser::enterState(StateType state) {
    context_->getStateManager().pushState(state);
}

void Parser::exitState() {
    context_->getStateManager().popState();
}

bool Parser::isInState(StateType state) const {
    return context_->getStateManager().isInState(state);
}

// 以下是其他解析方法的简化实现，完整实现需要根据语法文档完善

std::shared_ptr<ASTNode> Parser::parseTemplate() {
    auto location = current_->getLocation();
    consume(TokenType::KEYWORD_TEMPLATE, "Expected '[Template]'");
    
    TemplateType type;
    if (match(TokenType::TYPE_STYLE)) {
        type = TemplateType::STYLE;
    } else if (match(TokenType::TYPE_ELEMENT)) {
        type = TemplateType::ELEMENT;
    } else if (match(TokenType::TYPE_VAR)) {
        type = TemplateType::VAR;
    } else {
        error("Expected template type (@Style, @Element, or @Var)");
        return nullptr;
    }
    
    std::string name = parseIdentifier();
    consume(TokenType::LEFT_BRACE, "Expected '{' after template name");
    
    auto templateNode = std::make_shared<TemplateNode>(type, name, location);
    
    enterState(StateType::IN_TEMPLATE);
    
    // 解析模板内容
    auto content = parseTemplateContent(type);
    if (content) {
        templateNode->setContent(content);
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after template content");
    
    exitState();
    
    // 注册到全局映射表
    GlobalMap::getInstance().registerTemplate(name, 
        type == TemplateType::STYLE ? TemplateInfo::TemplateKind::STYLE :
        type == TemplateType::ELEMENT ? TemplateInfo::TemplateKind::ELEMENT :
        TemplateInfo::TemplateKind::VAR,
        context_->getSourceFile());
    
    return templateNode;
}

std::shared_ptr<ASTNode> Parser::parseCustom() {
    auto location = current_->getLocation();
    consume(TokenType::KEYWORD_CUSTOM, "Expected '[Custom]'");
    
    // 检查类型 @Style, @Element, @Var
    CustomType type;
    if (match(TokenType::TYPE_STYLE)) {
        type = CustomType::STYLE;
    } else if (match(TokenType::TYPE_ELEMENT)) {
        type = CustomType::ELEMENT;
    } else if (match(TokenType::TYPE_VAR)) {
        type = CustomType::VAR;
    } else {
        error("Expected custom type (@Style, @Element, or @Var)");
        return nullptr;
    }
    
    // 获取名称
    std::string name = parseIdentifier();
    
    auto customNode = std::make_shared<CustomNode>(type, name, location);
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after custom name");
    
    enterState(StateType::IN_CUSTOM);
    
    // 解析自定义内容
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto content = parseCustomContent(type);
        if (content) {
            customNode->addContent(content);
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after custom content");
    
    exitState();
    
    // 注册到全局映射表
    GlobalMap::getInstance().registerCustom(name, 
        type == CustomType::STYLE ? CustomInfo::CustomKind::STYLE :
        type == CustomType::ELEMENT ? CustomInfo::CustomKind::ELEMENT :
        CustomInfo::CustomKind::VAR,
        context_->getSourceFile());
    
    return customNode;
}

std::shared_ptr<ASTNode> Parser::parseOrigin() {
    auto location = current_->getLocation();
    consume(TokenType::KEYWORD_ORIGIN, "Expected '[Origin]'");
    
    // 检查类型 @Html, @Style, @JavaScript, 或自定义类型
    OriginType type = OriginType::HTML;
    std::string customType;
    
    if (match(TokenType::TYPE_HTML)) {
        type = OriginType::HTML;
    } else if (match(TokenType::TYPE_STYLE)) {
        type = OriginType::STYLE;
    } else if (match(TokenType::TYPE_JAVASCRIPT)) {
        type = OriginType::JAVASCRIPT;
    } else if (check(TokenType::AT) && peek(1) && peek(1)->getType() == TokenType::IDENTIFIER) {
        // Custom origin type like @Vue
        advance(); // skip @
        customType = parseIdentifier();
        type = OriginType::CUSTOM;
    } else {
        error("Expected origin type (@Html, @Style, @JavaScript, or custom)");
        return nullptr;
    }
    
    // 可选的名称
    std::string name;
    if (check(TokenType::IDENTIFIER)) {
        name = parseIdentifier();
    }
    
    auto originNode = std::make_shared<OriginNode>(type, name, location);
    if (type == OriginType::CUSTOM_ORIGIN) {
        // TODO: Add custom type support to OriginNode
        // originNode->setCustomType(customType);
    }
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after origin type");
    
    enterState(StateType::IN_ORIGIN);
    
    // 收集原始内容直到 }
    std::string content;
    int braceDepth = 0;
    
    while (!isAtEnd()) {
        if (check(TokenType::LEFT_BRACE)) {
            braceDepth++;
        } else if (check(TokenType::RIGHT_BRACE)) {
            if (braceDepth == 0) break;
            braceDepth--;
        }
        
        content += current_->getLexeme();
        if (check(TokenType::NEWLINE)) {
            content += "\n";
        } else if (!isAtEnd()) {
            content += " ";
        }
        advance();
    }
    
    originNode->setContent(content);
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after origin content");
    
    exitState();
    
    return originNode;
}

std::shared_ptr<ASTNode> Parser::parseImport() {
    auto location = current_->getLocation();
    consume(TokenType::KEYWORD_IMPORT, "Expected '[Import]'");
    
    ImportType importType = ImportType::CHTL;
    std::string targetName;
    std::string asName;
    std::string fromPath;
    
    // Check for [Custom] or [Template] or [Origin] or [Configuration]
    // bool isQualified = false;
    if (match(TokenType::KEYWORD_CUSTOM)) {
        // isQualified = true;
    } else if (match(TokenType::KEYWORD_TEMPLATE)) {
        // isQualified = true;
    } else if (match(TokenType::KEYWORD_ORIGIN)) {
        // isQualified = true;
    } else if (match(TokenType::KEYWORD_CONFIGURATION)) {
        // isQualified = true;
    }
    
    // Parse import type
    if (match(TokenType::TYPE_HTML)) {
        importType = ImportType::HTML;
    } else if (match(TokenType::TYPE_STYLE)) {
        importType = ImportType::STYLE;
    } else if (match(TokenType::TYPE_JAVASCRIPT)) {
        importType = ImportType::JAVASCRIPT;
    } else if (match(TokenType::TYPE_CHTL)) {
        importType = ImportType::CHTL;
    } else if (match(TokenType::TYPE_CJMOD)) {
        importType = ImportType::CJMOD;
    } else if (match(TokenType::TYPE_CONFIG)) {
        importType = ImportType::CONFIG;
    } else if (match(TokenType::TYPE_ELEMENT)) {
        // For qualified imports like [Import] [Custom] @Element
        if (check(TokenType::IDENTIFIER)) {
            targetName = parseIdentifier();
        }
    } else if (match(TokenType::TYPE_VAR)) {
        // For qualified imports like [Import] [Custom] @Var
        if (check(TokenType::IDENTIFIER)) {
            targetName = parseIdentifier();
        }
    }
    
    // Check for specific import target
    if (targetName.empty() && check(TokenType::IDENTIFIER)) {
        targetName = parseIdentifier();
    }
    
    // Parse 'from' clause
    if (match(TokenType::KEYWORD_FROM)) {
        if (check(TokenType::STRING_LITERAL)) {
            fromPath = parseString();
        } else if (check(TokenType::IDENTIFIER) || check(TokenType::UNQUOTED_LITERAL)) {
            fromPath = parseUnquotedLiteral();
        } else {
            error("Expected path after 'from'");
        }
    } else {
        error("Expected 'from' in import statement");
    }
    
    // Parse optional 'as' clause
    if (match(TokenType::KEYWORD_AS)) {
        asName = parseIdentifier();
    }
    
    auto importNode = std::make_shared<ImportNode>(importType, fromPath, location);
    // TODO: Add target name and as name support to ImportNode
    // importNode->setTargetName(targetName);
    // importNode->setAsName(asName);
    
    return importNode;
}

std::shared_ptr<ASTNode> Parser::parseNamespace() {
    auto location = current_->getLocation();
    consume(TokenType::KEYWORD_NAMESPACE, "Expected '[Namespace]'");
    
    std::string name = parseIdentifier();
    
    auto namespaceNode = std::make_shared<NamespaceNode>(name, location);
    
    // Namespace can optionally have braces
    bool hasBraces = false;
    if (match(TokenType::LEFT_BRACE)) {
        hasBraces = true;
    }
    
    enterState(StateType::IN_NAMESPACE);
    
    if (hasBraces) {
        // Parse namespace content
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            auto stmt = parseStatement();
            if (stmt) {
                namespaceNode->addContent(stmt);
            }
        }
        
        consume(TokenType::RIGHT_BRACE, "Expected '}' after namespace content");
    }
    
    exitState();
    
    // Register namespace
    context_->enterNamespace(name);
    
    return namespaceNode;
}

std::shared_ptr<ASTNode> Parser::parseConfiguration() {
    auto location = current_->getLocation();
    consume(TokenType::KEYWORD_CONFIGURATION, "Expected '[Configuration]'");
    
    std::string name;
    
    // Optional @Config name
    if (match(TokenType::TYPE_CONFIG)) {
        if (check(TokenType::IDENTIFIER)) {
            name = parseIdentifier();
        }
    }
    
    auto configNode = std::make_shared<ConfigNode>(name, location);
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after configuration");
    
    enterState(StateType::IN_CONFIGURATION);
    
    // Parse configuration options
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (check(TokenType::IDENTIFIER)) {
            std::string key = parseIdentifier();
            
            if (match(TokenType::EQUAL)) {
                std::string value = parseAttributeValue();
                // TODO: Add option support to ConfigNode
                // configNode->addOption(key, value);
                match(TokenType::SEMICOLON);
            }
        } else if (match(TokenType::LEFT_BRACKET)) {
            // Nested configuration section like [Name]
            std::string sectionName = parseIdentifier();
            consume(TokenType::RIGHT_BRACKET, "Expected ']' after section name");
            consume(TokenType::LEFT_BRACE, "Expected '{' after section");
            
            // Parse section content
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (check(TokenType::IDENTIFIER)) {
                    std::string key = parseIdentifier();
                    if (match(TokenType::EQUAL)) {
                        parseAttributeValue();
                        match(TokenType::SEMICOLON);
                    }
                } else {
                    advance();
                }
            }
            
            consume(TokenType::RIGHT_BRACE, "Expected '}' after section");
        } else {
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after configuration");
    
    exitState();
    
    return configNode;
}

std::shared_ptr<ASTNode> Parser::parseInfo() {
    // 解析信息块
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::parseExport() {
    // 解析导出块
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::parseTemplateContent(TemplateType type) {
    switch (type) {
        case TemplateType::STYLE: {
            // Parse style template content (CSS properties)
            auto styleNode = std::make_shared<StyleNode>(StyleBlockType::GLOBAL, current_->getLocation());
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                // For now, skip style content
                if (check(TokenType::SEMICOLON)) {
                    advance();
                } else {
                    advance();
                }
            }
            return styleNode;
        }
        
        case TemplateType::ELEMENT: {
            // Parse element template content (nested elements)
            std::vector<std::shared_ptr<ASTNode>> elements;
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (check(TokenType::HTML_TAG) || check(TokenType::IDENTIFIER)) {
                    std::string name = current_->getLexeme();
                    if (isHtmlTag(name)) {
                        auto elem = parseElement();
                        if (elem) {
                            elements.push_back(elem);
                        }
                    }
                } else {
                    advance();
                }
            }
            
            // Return a container node with all elements
            if (!elements.empty()) {
                return elements[0]; // TODO: Support multiple elements
            }
            break;
        }
        
        case TemplateType::VAR: {
            // Parse variable template content (key-value pairs)
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (check(TokenType::IDENTIFIER)) {
                    std::string varName = parseIdentifier();
                    if (match({TokenType::COLON, TokenType::EQUAL})) {
                        parseAttributeValue();
                        consume(TokenType::SEMICOLON, "Expected ';' after variable value");
                    }
                } else {
                    advance();
                }
            }
            break;
        }
    }
    
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::parseStyleContent() {
    // For now, collect raw CSS content for the style block
    // This should be enhanced to parse CSS selectors, properties, etc.
    
    // Skip any whitespace
    while (match({TokenType::NEWLINE, TokenType::WHITESPACE})) {
        // Skip
    }
    
    // Check for selector or property
    if (check(TokenType::IDENTIFIER) || check(TokenType::DOT) || 
        check(TokenType::HASH) || check(TokenType::AMPERSAND)) {
        // For now, skip to the next }
        int braceDepth = 0;
        while (!isAtEnd()) {
            if (check(TokenType::LEFT_BRACE)) {
                braceDepth++;
                advance();
            } else if (check(TokenType::RIGHT_BRACE)) {
                if (braceDepth == 0) {
                    break;
                }
                braceDepth--;
                advance();
            } else {
                advance();
            }
        }
    }
    
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::parseTemplateUse() {
    // 解析模板使用
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::parseCustomContent(CustomType type) {
    switch (type) {
        case CustomType::STYLE: {
            // Parse style custom content (CSS properties, inherit, delete)
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (match(TokenType::KEYWORD_DELETE)) {
                    // Parse delete operation
                    advance();
                } else if (match(TokenType::KEYWORD_INHERIT)) {
                    // Parse inherit operation
                    advance();
                } else if (check(TokenType::TYPE_STYLE)) {
                    // Parse style group usage
                    advance();
                } else if (check(TokenType::IDENTIFIER)) {
                    // Parse property or selector
                    advance();
                } else {
                    advance();
                }
            }
            break;
        }
        
        case CustomType::ELEMENT: {
            // Parse element custom content
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (match(TokenType::KEYWORD_DELETE)) {
                    // Parse delete operation
                    advance();
                } else if (match(TokenType::KEYWORD_INSERT)) {
                    // Parse insert operation
                    advance();
                } else if (check(TokenType::HTML_TAG) || check(TokenType::IDENTIFIER)) {
                    std::string name = current_->getLexeme();
                    if (isHtmlTag(name)) {
                        auto elem = parseElement();
                        if (elem) {
                            return elem;
                        }
                    }
                } else {
                    advance();
                }
            }
            break;
        }
        
        case CustomType::VAR: {
            // Parse variable custom content
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (check(TokenType::IDENTIFIER)) {
                    std::string varName = parseIdentifier();
                    if (match({TokenType::COLON, TokenType::EQUAL})) {
                        parseAttributeValue();
                        match(TokenType::SEMICOLON);
                    } else if (match(TokenType::COMMA)) {
                        // Variable without value
                        continue;
                    } else if (match(TokenType::SEMICOLON)) {
                        // Variable without value
                        continue;
                    }
                } else {
                    advance();
                }
            }
            break;
        }
    }
    
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::parseExceptConstraint() {
    auto location = current_->getLocation();
    consume(TokenType::KEYWORD_EXCEPT, "Expected 'except'");
    
    auto exceptNode = std::make_shared<ExceptNode>(location);
    
    // Parse constraint targets
    do {
        if (check(TokenType::HTML_TAG) || check(TokenType::IDENTIFIER)) {
            // Specific element constraint
            std::string name = parseIdentifier();
            exceptNode->addTarget(name);
        } else if (check(TokenType::TYPE_HTML)) {
            // Type constraint like @Html
            advance();
            exceptNode->addTypeConstraint("@Html");
        } else if (match(TokenType::KEYWORD_TEMPLATE)) {
            // [Template] constraint
            if (check(TokenType::TYPE_STYLE) || check(TokenType::TYPE_ELEMENT) || 
                check(TokenType::TYPE_VAR)) {
                advance();
            }
            exceptNode->addTypeConstraint("[Template]");
        } else if (match(TokenType::KEYWORD_CUSTOM)) {
            // [Custom] constraint
            if (check(TokenType::TYPE_STYLE) || check(TokenType::TYPE_ELEMENT) || 
                check(TokenType::TYPE_VAR)) {
                advance();
            }
            exceptNode->addTypeConstraint("[Custom]");
        } else {
            break;
        }
    } while (match(TokenType::COMMA));
    
    consume(TokenType::SEMICOLON, "Expected ';' after except constraint");
    
    return exceptNode;
}

} // namespace CHTL