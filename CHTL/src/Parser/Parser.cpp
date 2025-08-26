#include "CHTL/Parser/Parser.h"

namespace CHTL {

// In ParseCustomElementSpecialization or similar function
AstNodePtr Parser::ParseDeleteStatement() {
    Consume(TokenType::Delete, "Expect 'delete' keyword.");
    auto deleteNode = std::make_shared<ElementDeleteNode>();

    Token target = Peek();

    if (target.Type == TokenType::TypeIdentifier) {
        // Inherited element deletion: delete @Element Box;
        Advance(); // Consume @Element
        deleteNode->DeleteType = DeleteType::InheritedElement;
        deleteNode->TargetTypeIdentifier = target.Lexeme;
        deleteNode->TargetName = Consume(TokenType::Identifier, "Expect inherited element name.").Lexeme;
    } else if (target.Type == TokenType::Identifier) {
        deleteNode->TargetName = Advance().Lexeme;
        if (Match(TokenType::LBracket)) {
            // Indexed deletion: delete div[1];
            deleteNode->DeleteType = DeleteType::ElementIndexed;
            Token indexToken = Consume(TokenType::Number, "Expect index number.");
            deleteNode->Index = std::stoi(indexToken.Lexeme);
            Consume(TokenType::RBracket, "Expect ']'.");
        } else {
            // Deletion by name: delete span;
            deleteNode->DeleteType = DeleteType::ElementNamed;
        }
    } else {
        Error(target, "Unexpected token after 'delete'. Expect element name or type identifier.");
    }

    Consume(TokenType::Semicolon, "Expect ';' after delete statement.");
    return deleteNode;
}

AstNodePtr Parser::ParseDeclaration() {
    // Check for top-level declarations
    if (Check(TokenType::Use)) {
        return ParseUseStatement();
    } else if (Check(TokenType::LBracket)) {
        // Could be [Template], [Custom], [Origin], [Import], [Namespace], [Configuration]
        size_t savedPos = m_Current;
        Advance(); // consume [
        
        if (Check(TokenType::Template)) {
            m_Current = savedPos; // restore position
            return ParseTemplateDefinition();
        } else if (Check(TokenType::Custom)) {
            m_Current = savedPos;
            return ParseCustomDefinition();
        } else if (Check(TokenType::Origin)) {
            m_Current = savedPos;
            return ParseOriginNode();
        } else if (Check(TokenType::Import)) {
            m_Current = savedPos;
            return ParseImportNode();
        } else if (Check(TokenType::Namespace)) {
            m_Current = savedPos;
            return ParseNamespaceNode();
        } else if (Check(TokenType::Configuration)) {
            m_Current = savedPos;
            // Parse config and update the current config
            ParseConfigNode(m_Config);
            return nullptr; // Config nodes don't return AST nodes
        } else {
            m_Current = savedPos;
            // Fall through to element parsing
        }
    }
    
    // Default to element parsing
    return ParseElement();
}

// ... other necessary parser functions

AstNodePtr Parser::ParseUseStatement() {
    Consume(TokenType::Use, "Expect 'use' keyword.");
    auto useNode = std::make_shared<UseNode>();
    
    if (Match({TokenType::Html5})) {
        // use html5;
        useNode->Type = UseNode::Html5;
    } else if (Match({TokenType::At})) {
        // use @Config ConfigName;
        Token typeToken = Consume(TokenType::Identifier, "Expect type identifier after '@'.");
        if (typeToken.Lexeme == "Config" || typeToken.Lexeme == "Configuration") {
            useNode->Type = UseNode::Config;
            
            // Config name is optional
            if (Peek().Type == TokenType::Identifier) {
                useNode->ConfigName = Advance().Lexeme;
            }
        } else {
            Error(typeToken, "Invalid type after '@' in use statement. Expected 'Config'.");
        }
    } else if (Match({TokenType::LBracket})) {
        // use [Configuration] @Config ConfigName; (full prefix)
        Token keyword = Consume(TokenType::Configuration, "Expect 'Configuration' after '['.");
        Consume(TokenType::RBracket, "Expect ']' after 'Configuration'.");
        Consume(TokenType::At, "Expect '@' after '[Configuration]'.");
        Token typeToken = Consume(TokenType::Identifier, "Expect 'Config' after '@'.");
        
        if (typeToken.Lexeme == "Config") {
            useNode->Type = UseNode::Config;
            
            // Config name is optional
            if (Peek().Type == TokenType::Identifier) {
                useNode->ConfigName = Advance().Lexeme;
            }
        } else {
            Error(typeToken, "Invalid type identifier. Expected 'Config'.");
        }
    } else {
        Error(Peek(), "Invalid use statement. Expected 'html5' or '@Config'.");
    }
    
    Consume(TokenType::Semicolon, "Expect ';' after use statement.");
    return useNode;
}

}
