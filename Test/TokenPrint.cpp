#include "TokenPrint.h"
#include <iostream>

namespace CHTL {
namespace Test {

void TokenPrint::Print(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        std::cout << "[" << GetTokenTypeName(token.Type) << "] "
                  << "Lexeme: '" << token.Lexeme << "' "
                  << "Line: " << token.Line << std::endl;
    }
}

const char* TokenPrint::GetTokenTypeName(TokenType type) {
    switch (type) {
        case TokenType::Identifier: return "Identifier";
        case TokenType::String: return "String";
        case TokenType::Number: return "Number";
        // ... all other token types
        default: return "Unknown";
    }
}

}
}
