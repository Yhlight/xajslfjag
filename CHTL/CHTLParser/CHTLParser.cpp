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
        state.popState();
        return nullptr;
    }
    
    std::string template_name;
    if (check(TokenType::IDENTIFIER)) {
        template_name = advance().value;
    } else {
        addError("Expected template name");
        state.popState();
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
    return check(TokenType::CUSTOM) || 
           (check(TokenType::AT_STYLE) || check(TokenType::AT_ELEMENT) || check(TokenType::AT_VAR));
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
    consume(TokenType::CUSTOM, "Expected '[Custom]'");
    state.pushState(ParsingState::InCustomDefinition);
    
    std::string custom_type;
    if (check(TokenType::AT_STYLE)) {
        custom_type = "Style";
        advance();
    } else if (check(TokenType::AT_ELEMENT)) {
        custom_type = "Element";
        advance();
    } else if (check(TokenType::AT_VAR)) {
        custom_type = "Var";
        advance();
    } else {
        addError("Expected custom type (@Style, @Element, @Var)");
        state.popState();
        return nullptr;
    }
    
    std::string custom_name;
    if (check(TokenType::IDENTIFIER)) {
        custom_name = advance().value;
    } else {
        addError("Expected custom name");
        state.popState();
        return nullptr;
    }
    
    auto custom_node = NodeFactory::createCustom(custom_type, custom_name);
    context.setCurrentCustom(custom_name);
    
    consume(TokenType::LBRACE, "Expected '{' after custom declaration");
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (check(TokenType::NEWLINE)) {
            advance();
            continue;
        }
        
        std::shared_ptr<CHTLNode> child = nullptr;
        
        if (custom_type == "Element") {
            if (isElement()) {
                child = parseElement();
            } else if (isTemplateUsage() || isCustomUsage()) {
                child = parseTemplateUsage(); // Will handle both template and custom usage
            } else {
                addError("Invalid content in custom element definition");
                synchronize();
                continue;
            }
        } else if (custom_type == "Style") {
            if (check(TokenType::IDENTIFIER)) {
                // Parse style property or inheritance
                Token lookahead = peek(1);
                if (lookahead.type == TokenType::COLON) {
                    child = parseStyleProperty();
                } else if (lookahead.type == TokenType::COMMA || lookahead.type == TokenType::SEMICOLON) {
                    // Parse property list for Custom @Style without values
                    auto prop_node = std::make_shared<CHTLNode>(NodeType::ATTRIBUTE);
                    prop_node->name = advance().value;
                    prop_node->value = ""; // No value for custom style properties
                    
                    // Handle comma-separated properties
                    while (match(TokenType::COMMA)) {
                        if (check(TokenType::IDENTIFIER)) {
                            auto another_prop = std::make_shared<CHTLNode>(NodeType::ATTRIBUTE);
                            another_prop->name = advance().value;
                            another_prop->value = "";
                            custom_node->addChild(another_prop);
                        }
                    }
                    
                    consume(TokenType::SEMICOLON, "Expected ';' after property list");
                    child = prop_node;
                } else {
                    addError("Invalid content in custom style definition");
                    synchronize();
                    continue;
                }
            } else if (isTemplateUsage() || isCustomUsage()) {
                child = parseTemplateUsage(); // Handle inheritance
            } else {
                addError("Invalid content in custom style definition");
                synchronize();
                continue;
            }
        } else if (custom_type == "Var") {
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
            } else {
                addError("Invalid content in custom variable definition");
                synchronize();
                continue;
            }
        }
        
        if (child) {
            custom_node->addChild(child);
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}' after custom body");
    state.popState();
    context.setCurrentCustom("");
    
    return custom_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseOriginDefinition() {
    consume(TokenType::ORIGIN, "Expected '[Origin]'");
    state.pushState(ParsingState::InOriginDefinition);
    
    std::string origin_type;
    if (check(TokenType::AT_HTML)) {
        origin_type = "Html";
        advance();
    } else if (check(TokenType::AT_STYLE)) {
        origin_type = "Style";
        advance();
    } else if (check(TokenType::AT_JAVASCRIPT)) {
        origin_type = "JavaScript";
        advance();
    } else if (check(TokenType::IDENTIFIER) && peek().value.length() > 0 && peek().value[0] == '@') {
        // Custom origin type like @Vue
        origin_type = advance().value;
    } else {
        addError("Expected origin type (@Html, @Style, @JavaScript, or custom @Type)");
        state.popState();
        return nullptr;
    }
    
    std::string origin_name;
    if (check(TokenType::IDENTIFIER)) {
        origin_name = advance().value;
    }
    
    auto origin_node = std::make_shared<CHTLNode>(NodeType::ORIGIN_DEFINITION);
    origin_node->name = origin_name;
    origin_node->setAttribute("type", origin_type);
    
    if (match(TokenType::LBRACE)) {
        // Parse raw content
        std::string raw_content;
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            Token token = advance();
            if (token.type == TokenType::NEWLINE) {
                raw_content += "\n";
            } else {
                raw_content += token.value;
            }
        }
        
        origin_node->value = raw_content;
        consume(TokenType::RBRACE, "Expected '}' after origin content");
    } else if (match(TokenType::SEMICOLON)) {
        // Usage form: [Origin] @Html name;
        origin_node->setAttribute("usage", "true");
    }
    
    state.popState();
    return origin_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseConfigurationDefinition() {
    consume(TokenType::CONFIGURATION, "Expected '[Configuration]'");
    state.pushState(ParsingState::InConfigurationDefinition);
    
    auto config_node = std::make_shared<CHTLNode>(NodeType::CONFIGURATION_DEFINITION);
    
    consume(TokenType::LBRACE, "Expected '{' after [Configuration]");
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (check(TokenType::NEWLINE)) {
            advance();
            continue;
        }
        
        if (check(TokenType::IDENTIFIER)) {
            std::string config_key = advance().value;
            consume(TokenType::COLON, "Expected ':' after configuration key");
            
            std::string config_value;
            if (check(TokenType::STRING_LITERAL)) {
                config_value = advance().value;
            } else if (check(TokenType::NUMBER)) {
                config_value = advance().value;
            } else if (check(TokenType::IDENTIFIER)) {
                config_value = advance().value;
            } else {
                addError("Expected configuration value");
                synchronize();
                continue;
            }
            
            consume(TokenType::SEMICOLON, "Expected ';' after configuration value");
            
            auto config_item = std::make_shared<CHTLNode>(NodeType::ATTRIBUTE);
            config_item->name = config_key;
            config_item->value = config_value;
            config_node->addChild(config_item);
            
            // Set configuration in context
            context.setConfig(config_key, config_value);
        } else {
            addError("Expected configuration key");
            synchronize();
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}' after configuration body");
    state.popState();
    
    return config_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseImportStatement() {
    consume(TokenType::IMPORT, "Expected '[Import]'");
    state.pushState(ParsingState::InImportStatement);
    
    std::string import_type;
    if (check(TokenType::AT_HTML)) {
        import_type = "Html";
        advance();
    } else if (check(TokenType::AT_STYLE)) {
        import_type = "Style";
        advance();
    } else if (check(TokenType::AT_JAVASCRIPT)) {
        import_type = "JavaScript";
        advance();
    } else if (check(TokenType::AT_CHTL)) {
        import_type = "Chtl";
        advance();
    } else if (check(TokenType::AT_CJMOD)) {
        import_type = "CJmod";
        advance();
    } else {
        addError("Expected import type (@Html, @Style, @JavaScript, @Chtl, @CJmod)");
        state.popState();
        return nullptr;
    }
    
    std::string import_path;
    if (check(TokenType::STRING_LITERAL)) {
        import_path = advance().value;
    } else if (check(TokenType::IDENTIFIER)) {
        import_path = advance().value;
    } else {
        addError("Expected import path");
        state.popState();
        return nullptr;
    }
    
    auto import_node = NodeFactory::createImport(import_type, import_path);
    
    // Handle optional 'as' clause
    if (check(TokenType::AS)) {
        advance();
        if (check(TokenType::IDENTIFIER)) {
            std::string alias = advance().value;
            import_node->setAttribute("alias", alias);
        } else {
            addError("Expected alias name after 'as'");
        }
    }
    
    // Handle optional 'except' clause
    if (check(TokenType::EXCEPT)) {
        advance();
        state.pushState(ParsingState::InExceptClause);
        
        consume(TokenType::LBRACE, "Expected '{' after 'except'");
        
        std::string except_items;
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            if (check(TokenType::IDENTIFIER)) {
                if (!except_items.empty()) except_items += ",";
                except_items += advance().value;
                
                if (match(TokenType::COMMA)) {
                    // Continue with next item
                }
            } else if (check(TokenType::NEWLINE)) {
                advance();
            } else {
                addError("Expected identifier in except clause");
                break;
            }
        }
        
        consume(TokenType::RBRACE, "Expected '}' after except clause");
        import_node->setAttribute("except", except_items);
        
        state.popState();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after import statement");
    state.popState();
    
    return import_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseScriptBlock() {
    consume(TokenType::SCRIPT, "Expected 'script'");
    state.pushState(ParsingState::InScriptBlock);
    
    auto script_node = NodeFactory::createScript(true); // local script
    consume(TokenType::LBRACE, "Expected '{' after 'script'");
    
    std::string script_content;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        Token token = advance();
        if (token.type == TokenType::NEWLINE) {
            script_content += "\n";
        } else {
            script_content += token.value + " ";
        }
    }
    
    script_node->value = script_content;
    consume(TokenType::RBRACE, "Expected '}' after script block");
    state.popState();
    
    return script_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseTemplateUsage() {
    std::string usage_type;
    NodeType node_type;
    
    if (check(TokenType::AT_STYLE)) {
        usage_type = "Style";
        node_type = NodeType::TEMPLATE_USAGE;
        advance();
    } else if (check(TokenType::AT_ELEMENT)) {
        usage_type = "Element";
        node_type = NodeType::TEMPLATE_USAGE;
        advance();
    } else if (check(TokenType::AT_VAR)) {
        usage_type = "Var";
        node_type = NodeType::VARIABLE_USAGE;
        advance();
    } else if (check(TokenType::TEMPLATE)) {
        // Full qualified name: [Template] @Style Name
        advance(); // consume TEMPLATE
        if (check(TokenType::AT_STYLE)) {
            usage_type = "Style";
            node_type = NodeType::TEMPLATE_USAGE;
            advance();
        } else if (check(TokenType::AT_ELEMENT)) {
            usage_type = "Element";
            node_type = NodeType::TEMPLATE_USAGE;
            advance();
        } else if (check(TokenType::AT_VAR)) {
            usage_type = "Var";
            node_type = NodeType::VARIABLE_USAGE;
            advance();
        } else {
            addError("Expected template type after [Template]");
            return nullptr;
        }
    } else if (check(TokenType::CUSTOM)) {
        // Custom usage: [Custom] @Style Name
        advance(); // consume CUSTOM
        if (check(TokenType::AT_STYLE)) {
            usage_type = "Style";
            node_type = NodeType::CUSTOM_USAGE;
            advance();
        } else if (check(TokenType::AT_ELEMENT)) {
            usage_type = "Element";
            node_type = NodeType::CUSTOM_USAGE;
            advance();
        } else if (check(TokenType::AT_VAR)) {
            usage_type = "Var";
            node_type = NodeType::CUSTOM_USAGE;
            advance();
        } else {
            addError("Expected custom type after [Custom]");
            return nullptr;
        }
    } else {
        addError("Expected template or custom usage");
        return nullptr;
    }
    
    std::string usage_name;
    if (check(TokenType::IDENTIFIER)) {
        usage_name = advance().value;
    } else {
        addError("Expected template/custom name");
        return nullptr;
    }
    
    auto usage_node = std::make_shared<CHTLNode>(node_type);
    usage_node->name = usage_name;
    usage_node->value = usage_type;
    
    // Handle specialization block or variable parameters
    if (match(TokenType::LBRACE)) {
        // Specialization block for Custom usage
        state.pushState(ParsingState::InStyleSpecialization);
        
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            if (check(TokenType::NEWLINE)) {
                advance();
                continue;
            }
            
            std::shared_ptr<CHTLNode> child = nullptr;
            
            // Handle delete, insert, inherit operations
            if (check(TokenType::DELETE)) {
                advance();
                auto delete_node = std::make_shared<CHTLNode>(NodeType::ATTRIBUTE);
                delete_node->name = "delete";
                
                std::string delete_items;
                do {
                    if (check(TokenType::IDENTIFIER)) {
                        if (!delete_items.empty()) delete_items += ",";
                        delete_items += advance().value;
                    } else if (isTemplateUsage() || isCustomUsage()) {
                        // Delete template/custom inheritance
                        auto delete_ref = parseTemplateUsage();
                        if (delete_ref) {
                            delete_node->addChild(delete_ref);
                        }
                    }
                } while (match(TokenType::COMMA));
                
                delete_node->value = delete_items;
                consume(TokenType::SEMICOLON, "Expected ';' after delete statement");
                child = delete_node;
                
            } else if (check(TokenType::INSERT)) {
                advance();
                auto insert_node = std::make_shared<CHTLNode>(NodeType::ATTRIBUTE);
                insert_node->name = "insert";
                
                // Parse insert position
                std::string position;
                if (check(TokenType::AFTER)) {
                    position = "after";
                    advance();
                } else if (check(TokenType::BEFORE)) {
                    position = "before";
                    advance();
                } else if (check(TokenType::REPLACE)) {
                    position = "replace";
                    advance();
                }
                
                insert_node->setAttribute("position", position);
                
                // Parse target selector
                if (check(TokenType::IDENTIFIER)) {
                    std::string target = advance().value;
                    
                    // Handle array index [0], [1] etc.
                    if (match(TokenType::LBRACKET)) {
                        if (check(TokenType::NUMBER)) {
                            target += "[" + advance().value + "]";
                        }
                        consume(TokenType::RBRACKET, "Expected ']' after array index");
                    }
                    
                    insert_node->setAttribute("target", target);
                }
                
                consume(TokenType::LBRACE, "Expected '{' after insert statement");
                
                // Parse insert content
                while (!check(TokenType::RBRACE) && !isAtEnd()) {
                    if (check(TokenType::NEWLINE)) {
                        advance();
                        continue;
                    }
                    
                    if (isElement()) {
                        auto element = parseElement();
                        if (element) insert_node->addChild(element);
                    } else if (isTemplateUsage()) {
                        auto template_ref = parseTemplateUsage();
                        if (template_ref) insert_node->addChild(template_ref);
                    } else {
                        addError("Invalid content in insert block");
                        synchronize();
                        break;
                    }
                }
                
                consume(TokenType::RBRACE, "Expected '}' after insert content");
                child = insert_node;
                
            } else if (check(TokenType::INHERIT)) {
                advance();
                auto inherit_node = parseTemplateUsage();
                if (inherit_node) {
                    inherit_node->setAttribute("operation", "inherit");
                    child = inherit_node;
                }
                
            } else if (usage_type == "Style") {
                // Parse style properties or nested template usage
                if (check(TokenType::IDENTIFIER)) {
                    child = parseStyleProperty();
                } else if (isTemplateUsage()) {
                    child = parseTemplateUsage();
                }
            } else if (usage_type == "Element") {
                // Parse element modifications
                if (isElement()) {
                    child = parseElement();
                } else if (isTemplateUsage()) {
                    child = parseTemplateUsage();
                }
            } else if (usage_type == "Var") {
                // Parse variable assignments
                if (check(TokenType::IDENTIFIER)) {
                    auto var_assign = std::make_shared<CHTLNode>(NodeType::VARIABLE_USAGE);
                    var_assign->name = advance().value;
                    
                    consume(TokenType::EQUAL, "Expected '=' for variable assignment");
                    
                    var_assign->value = parseStringLiteral();
                    consume(TokenType::SEMICOLON, "Expected ';' after variable assignment");
                    child = var_assign;
                }
            }
            
            if (child) {
                usage_node->addChild(child);
            } else {
                addError("Invalid content in specialization block");
                synchronize();
            }
        }
        
        consume(TokenType::RBRACE, "Expected '}' after specialization block");
        state.popState();
        
    } else if (match(TokenType::LPAREN)) {
        // Variable usage with parameters: ThemeColor(tableColor)
        std::string parameters;
        while (!check(TokenType::RPAREN) && !isAtEnd()) {
            if (check(TokenType::IDENTIFIER)) {
                if (!parameters.empty()) parameters += ",";
                parameters += advance().value;
                
                // Handle variable assignment: tableColor = value
                if (match(TokenType::EQUAL)) {
                    parameters += "=";
                    parameters += parseStringLiteral();
                }
            } else if (check(TokenType::COMMA)) {
                advance();
            } else {
                addError("Invalid parameter in variable usage");
                break;
            }
        }
        
        consume(TokenType::RPAREN, "Expected ')' after variable parameters");
        usage_node->setAttribute("parameters", parameters);
        
    } else {
        // Simple usage, consume semicolon if present
        if (check(TokenType::SEMICOLON)) {
            advance();
        }
    }
    
    return usage_node;
}

std::shared_ptr<CHTLNode> CHTLParser::parseCustomUsage() {
    // Custom usage is now handled by parseTemplateUsage()
    return parseTemplateUsage();
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