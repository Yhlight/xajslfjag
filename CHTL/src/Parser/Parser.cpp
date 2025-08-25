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

// ... other necessary parser functions

}
