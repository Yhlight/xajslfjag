#include "CHTLParser.h"
#include <sstream>

namespace CHTL {

CHTLParser::CHTLParser() : current(0) {}

Token CHTLParser::peek(size_t offset) const {
    size_t index = current + offset;
    if (index >= tokens.size()) {
        return Token(TokenType::EOF_TOKEN);
    }
    return tokens[index];
}

Token CHTLParser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

bool CHTLParser::isAtEnd() const {
    return current >= tokens.size() || peek().type == TokenType::EOF_TOKEN;
}

bool CHTLParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool CHTLParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    
    addError(message);
    return peek();
}

void CHTLParser::addError(const std::string& message) {
    Token token = peek();
    std::stringstream ss;
    ss << "Parse error at line " << token.line << ", column " << token.column 
       << ": " << message << " (got '" << token.value << "')";
    errors.push_back(ss.str());
}

void CHTLParser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (tokens[current - 1].type == TokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::TEMPLATE:
            case TokenType::CUSTOM:
            case TokenType::ORIGIN:
            case TokenType::IMPORT:
            case TokenType::NAMESPACE:
            case TokenType::CONFIGURATION:
            case TokenType::USE:
                return;
            default:
                break;
        }
        
        advance();
    }
}

std::shared_ptr<CHTLNode> CHTLParser::parse(const std::vector<Token>& token_list) {
    tokens = token_list;
    current = 0;
    errors.clear();
    state.clear();
    context.clear();
    
    return parseDocument();
}

std::shared_ptr<CHTLNode> CHTLParser::parseString(const std::string& source) {
    CHTLLexer lexer(source);
    std::vector<Token> token_list = lexer.tokenize();
    
    if (lexer.hasErrors()) {
        for (const auto& error : lexer.getErrors()) {
            errors.push_back(error);
        }
        return nullptr;
    }
    
    return parse(token_list);
}

std::shared_ptr<CHTLNode> CHTLParser::parseDocument() {
    auto document = NodeFactory::createDocument();
    state.pushState(ParsingState::GlobalScope);
    
    // Parse use declaration if present
    if (check(TokenType::USE)) {
        auto use_decl = parseUseDeclaration();
        if (use_decl) {
            document->addChild(use_decl);
        }
    }
    
    // Parse declarations and elements
    while (!isAtEnd()) {
        try {
            if (check(TokenType::NEWLINE)) {
                advance();
                continue;
            }
            
            std::shared_ptr<CHTLNode> node = nullptr;
            
            if (isDeclaration()) {
                node = parseDeclaration();
            } else if (isElement()) {
                node = parseElement();
            } else if (check(TokenType::TEXT)) {
                node = parseTextNode();
            } else if (check(TokenType::LINE_COMMENT) || check(TokenType::BLOCK_COMMENT) || check(TokenType::GENERATOR_COMMENT)) {
                node = parseComment();
            } else {
                addError("Unexpected token at top level");
                synchronize();
                continue;
            }
            
            if (node) {
                document->addChild(node);
            }
        } catch (...) {
            synchronize();
        }
    }
    
    state.popState();
    return document;
}

std::shared_ptr<CHTLNode> CHTLParser::parseDeclaration() {
    if (check(TokenType::USE)) {
        return parseUseDeclaration();
    } else if (check(TokenType::NAMESPACE)) {
        return parseNamespaceDeclaration();
    } else if (check(TokenType::TEMPLATE)) {
        return parseTemplateDefinition();
    } else if (check(TokenType::CUSTOM)) {
        return parseCustomDefinition();
    } else if (check(TokenType::ORIGIN)) {
        return parseOriginDefinition();
    } else if (check(TokenType::CONFIGURATION)) {
        return parseConfigurationDefinition();
    } else if (check(TokenType::IMPORT)) {
        return parseImportStatement();
    }
    
    addError("Expected declaration");
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseUseDeclaration() {
    consume(TokenType::USE, "Expected 'use'");
    state.pushState(ParsingState::InUseStatement);
    
    auto use_node = std::make_shared<CHTLNode>(NodeType::USE_DECLARATION);
    
    if (check(TokenType::IDENTIFIER)) {
        Token name = advance();
        use_node->value = name.value;
        context.setConfig("use_declaration", name.value);
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after use declaration");
    state.popState();
    return use_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseNamespaceDeclaration() {
    consume(TokenType::NAMESPACE, "Expected '[Namespace]'");
    state.pushState(ParsingState::InNamespaceDefinition);
    
    auto namespace_node = std::make_shared<CHTLNode>(NodeType::NAMESPACE_DEFINITION);
    
    if (check(TokenType::IDENTIFIER)) {
        Token name = advance();
        namespace_node->name = name.value;
        context.setCurrentNamespace(name.value);
    }
    
    if (match(TokenType::LBRACE)) {
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            if (check(TokenType::NEWLINE)) {
                advance();
                continue;
            }
            
            auto child = parseDeclaration();
            if (child) {
                namespace_node->addChild(child);
            }
        }
        consume(TokenType::RBRACE, "Expected '}' after namespace body");
    }
    
    state.popState();
    return namespace_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseTemplateDefinition() {
    consume(TokenType::TEMPLATE, "Expected '[Template]'");
    state.pushState(ParsingState::InTemplateDefinition);
    
    std::string template_type;
    if (check(TokenType::AT_STYLE)) {
        template_type = "Style";
        advance();
    } else if (check(TokenType::AT_ELEMENT)) {
        template_type = "Element";
        advance();
    } else if (check(TokenType::AT_VAR)) {
        template_type = "Var";
        advance();
    } else {
        addError("Expected template type (@Style, @Element, @Var)");
        return nullptr;
    }
    
    std::string template_name;
    if (check(TokenType::IDENTIFIER)) {
        template_name = advance().value;
    } else {
        addError("Expected template name");
        return nullptr;
    }
    
    auto template_node = NodeFactory::createTemplate(template_type, template_name);
    context.setCurrentTemplate(template_name);
    
    consume(TokenType::LBRACE, "Expected '{' after template declaration");
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (check(TokenType::NEWLINE)) {
            advance();
            continue;
        }
        
        std::shared_ptr<CHTLNode> child = nullptr;
        
        if (template_type == "Element" && isElement()) {
            child = parseElement();
        } else if (template_type == "Style" && isStyleDeclaration()) {
            child = parseStyleRule();
        } else if (template_type == "Var") {
            // Parse variable assignments
            if (check(TokenType::IDENTIFIER)) {
                auto var_node = std::make_shared<CHTLNode>(NodeType::VARIABLE_USAGE);
                var_node->name = advance().value;
                
                consume(TokenType::COLON, "Expected ':' after variable name");
                
                if (check(TokenType::STRING_LITERAL)) {
                    var_node->value = advance().value;
                } else {
                    var_node->value = parseStringLiteral();
                }
                
                consume(TokenType::SEMICOLON, "Expected ';' after variable assignment");
                child = var_node;
            }
        } else {
            addError("Invalid content in template definition");
            synchronize();
            continue;
        }
        
        if (child) {
            template_node->addChild(child);
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}' after template body");
    state.popState();
    context.setCurrentTemplate("");
    
    return template_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseElement() {
    if (!check(TokenType::IDENTIFIER)) {
        addError("Expected element name");
        return nullptr;
    }
    
    Token element_name = advance();
    auto element = NodeFactory::createElement(element_name.value);
    state.pushState(ParsingState::InElementBody);
    context.setCurrentElement(element_name.value);
    
    consume(TokenType::LBRACE, "Expected '{' after element name");
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (check(TokenType::NEWLINE)) {
            advance();
            continue;
        }
        
        std::shared_ptr<CHTLNode> child = nullptr;
        
        if (check(TokenType::IDENTIFIER)) {
            // Could be attribute or nested element
            Token lookahead = peek(1);
            if (lookahead.type == TokenType::COLON || lookahead.type == TokenType::EQUAL) {
                // It's an attribute
                child = parseAttribute();
            } else if (lookahead.type == TokenType::LBRACE) {
                // It's a nested element
                child = parseElement();
            } else {
                addError("Unexpected token after identifier");
                synchronize();
                continue;
            }
        } else if (check(TokenType::TEXT)) {
            child = parseTextNode();
        } else if (check(TokenType::STYLE)) {
            child = parseStyleBlock();
        } else if (check(TokenType::SCRIPT)) {
            child = parseScriptBlock();
        } else if (isTemplateUsage()) {
            child = parseTemplateUsage();
        } else if (isCustomUsage()) {
            child = parseCustomUsage();
        } else {
            addError("Unexpected token in element body");
            synchronize();
            continue;
        }
        
        if (child) {
            element->addChild(child);
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}' after element body");
    state.popState();
    context.setCurrentElement("");
    
    return element;
}

std::shared_ptr<CHTLNode> CHTLParser::parseTextNode() {
    consume(TokenType::TEXT, "Expected 'text'");
    
    auto text_node = NodeFactory::createText("");
    consume(TokenType::LBRACE, "Expected '{' after 'text'");
    
    std::string content;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (check(TokenType::STRING_LITERAL)) {
            content += advance().value;
        } else if (check(TokenType::IDENTIFIER)) {
            content += advance().value;
        } else if (check(TokenType::NEWLINE)) {
            content += "\n";
            advance();
        } else {
            content += advance().value;
        }
    }
    
    text_node->value = content;
    consume(TokenType::RBRACE, "Expected '}' after text content");
    
    return text_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseStyleBlock() {
    consume(TokenType::STYLE, "Expected 'style'");
    state.pushState(ParsingState::InStyleBlock);
    
    auto style_node = NodeFactory::createStyle(true); // local style
    consume(TokenType::LBRACE, "Expected '{' after 'style'");
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (check(TokenType::NEWLINE)) {
            advance();
            continue;
        }
        
        auto rule = parseStyleRule();
        if (rule) {
            style_node->addChild(rule);
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}' after style block");
    state.popState();
    
    return style_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseAttribute() {
    if (!check(TokenType::IDENTIFIER)) {
        addError("Expected attribute name");
        return nullptr;
    }
    
    Token attr_name = advance();
    auto attr_node = std::make_shared<CHTLNode>(NodeType::ATTRIBUTE);
    attr_node->name = attr_name.value;
    
    if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
        std::string value;
        if (check(TokenType::STRING_LITERAL)) {
            value = advance().value;
        } else if (check(TokenType::IDENTIFIER)) {
            value = advance().value;
        } else {
            value = parseStringLiteral();
        }
        attr_node->value = value;
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after attribute");
    return attr_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseStyleRule() {
    auto rule_node = std::make_shared<CHTLNode>(NodeType::STYLE_RULE);
    
    // Parse selector or property
    if (check(TokenType::CLASS_SELECTOR) || check(TokenType::ID_SELECTOR) || check(TokenType::AMPERSAND)) {
        // It's a selector rule
        std::string selector;
        if (check(TokenType::CLASS_SELECTOR) || check(TokenType::ID_SELECTOR)) {
            selector = advance().value;
        } else if (check(TokenType::AMPERSAND)) {
            selector = "&";
            advance();
            if (check(TokenType::COLON)) {
                selector += advance().value; // pseudo-class
                if (check(TokenType::IDENTIFIER)) {
                    selector += advance().value;
                }
            }
        }
        
        rule_node->name = selector;
        
        consume(TokenType::LBRACE, "Expected '{' after selector");
        
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            if (check(TokenType::NEWLINE)) {
                advance();
                continue;
            }
            
            auto property = parseStyleProperty();
            if (property) {
                rule_node->addChild(property);
            }
        }
        
        consume(TokenType::RBRACE, "Expected '}' after selector rule");
    } else {
        // It's a direct property
        auto property = parseStyleProperty();
        return property;
    }
    
    return rule_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseStyleProperty() {
    if (!check(TokenType::IDENTIFIER)) {
        addError("Expected property name");
        return nullptr;
    }
    
    Token prop_name = advance();
    auto prop_node = std::make_shared<CHTLNode>(NodeType::ATTRIBUTE);
    prop_node->name = prop_name.value;
    
    consume(TokenType::COLON, "Expected ':' after property name");
    
    std::string value;
    while (!check(TokenType::SEMICOLON) && !check(TokenType::NEWLINE) && !isAtEnd()) {
        Token token = advance();
        if (!value.empty()) value += " ";
        value += token.value;
    }
    
    prop_node->value = value;
    
    if (match(TokenType::SEMICOLON)) {
        // Optional semicolon
    }
    
    return prop_node;
}

// Helper methods
bool CHTLParser::isDeclaration() const {
    return check(TokenType::USE) || check(TokenType::NAMESPACE) ||
           check(TokenType::TEMPLATE) || check(TokenType::CUSTOM) ||
           check(TokenType::ORIGIN) || check(TokenType::CONFIGURATION) ||
           check(TokenType::IMPORT);
}

bool CHTLParser::isElement() const {
    return check(TokenType::IDENTIFIER) && peek(1).type == TokenType::LBRACE;
}

bool CHTLParser::isTemplateUsage() const {
    return check(TokenType::AT_STYLE) || check(TokenType::AT_ELEMENT) || check(TokenType::AT_VAR);
}

bool CHTLParser::isCustomUsage() const {
    // Custom usage detection logic
    return false; // Simplified for now
}

bool CHTLParser::isStyleDeclaration() const {
    return check(TokenType::IDENTIFIER) && (peek(1).type == TokenType::COLON);
}

std::string CHTLParser::parseStringLiteral() {
    if (check(TokenType::STRING_LITERAL)) {
        return advance().value;
    } else if (check(TokenType::IDENTIFIER)) {
        return advance().value;
    }
    return "";
}

// Stub implementations for remaining methods
std::shared_ptr<CHTLNode> CHTLParser::parseCustomDefinition() {
    // Implementation similar to parseTemplateDefinition
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseOriginDefinition() {
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseConfigurationDefinition() {
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseImportStatement() {
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseScriptBlock() {
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseTemplateUsage() {
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseCustomUsage() {
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseComment() {
    TokenType type = peek().type;
    Token content = advance();
    
    bool is_generator = (type == TokenType::GENERATOR_COMMENT);
    return NodeFactory::createComment(content.value, is_generator);
}

void CHTLParser::reset() {
    current = 0;
    errors.clear();
    state.clear();
    context.clear();
}

// Stub implementations for validation methods
bool CHTLParser::validateElementContext() const { return true; }
bool CHTLParser::validateStyleContext() const { return true; }
bool CHTLParser::validateTemplateContext() const { return true; }
bool CHTLParser::validateCustomContext() const { return true; }

} // namespace CHTL