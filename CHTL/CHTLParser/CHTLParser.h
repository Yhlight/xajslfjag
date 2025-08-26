#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/CHTLNode.h"
#include "../CHTLState/CHTLState.h"
#include "../CHTLContext/CHTLContext.h"
#include <memory>
#include <vector>

namespace CHTL {

class CHTLParser {
private:
    std::vector<Token> tokens;
    size_t current;
    std::vector<std::string> errors;
    
    CHTLState state;
    CHTLContext context;
    
    Token peek(size_t offset = 0) const;
    Token advance();
    bool isAtEnd() const;
    bool match(TokenType type);
    bool check(TokenType type) const;
    Token consume(TokenType type, const std::string& message);
    
    void addError(const std::string& message);
    void synchronize();
    
    // Parsing methods
    std::shared_ptr<CHTLNode> parseDocument();
    std::shared_ptr<CHTLNode> parseDeclaration();
    std::shared_ptr<CHTLNode> parseUseDeclaration();
    std::shared_ptr<CHTLNode> parseNamespaceDeclaration();
    std::shared_ptr<CHTLNode> parseTemplateDefinition();
    std::shared_ptr<CHTLNode> parseCustomDefinition();
    std::shared_ptr<CHTLNode> parseOriginDefinition();
    std::shared_ptr<CHTLNode> parseConfigurationDefinition();
    std::shared_ptr<CHTLNode> parseImportStatement();
    
    std::shared_ptr<CHTLNode> parseElement();
    std::shared_ptr<CHTLNode> parseTextNode();
    std::shared_ptr<CHTLNode> parseStyleBlock();
    std::shared_ptr<CHTLNode> parseScriptBlock();
    
    std::shared_ptr<CHTLNode> parseTemplateUsage();
    std::shared_ptr<CHTLNode> parseCustomUsage();
    std::shared_ptr<CHTLNode> parseVariableUsage();
    
    std::shared_ptr<CHTLNode> parseAttributeList();
    std::shared_ptr<CHTLNode> parseAttribute();
    
    std::shared_ptr<CHTLNode> parseStyleRule();
    std::shared_ptr<CHTLNode> parseStyleProperty();
    std::shared_ptr<CHTLNode> parseSelector();
    
    std::shared_ptr<CHTLNode> parseSpecialization();
    std::shared_ptr<CHTLNode> parseInheritance();
    std::shared_ptr<CHTLNode> parseConstraint();
    
    std::shared_ptr<CHTLNode> parseComment();
    std::shared_ptr<CHTLNode> parseLiteral();
    std::shared_ptr<CHTLNode> parseIdentifier();
    
    // Helper methods
    bool isDeclaration() const;
    bool isElement() const;
    bool isTemplateUsage() const;
    bool isCustomUsage() const;
    bool isStyleDeclaration() const;
    bool isScriptDeclaration() const;
    
    std::string parseStringLiteral();
    std::string parseIdentifierValue();
    std::vector<std::string> parseParameterList();
    
    // Validation methods
    bool validateElementContext() const;
    bool validateStyleContext() const;
    bool validateTemplateContext() const;
    bool validateCustomContext() const;
    
public:
    CHTLParser();
    
    std::shared_ptr<CHTLNode> parse(const std::vector<Token>& tokens);
    std::shared_ptr<CHTLNode> parseString(const std::string& source);
    
    const std::vector<std::string>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    
    void reset();
};

} // namespace CHTL

#endif // CHTL_PARSER_H