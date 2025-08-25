#include "CHTL/Lexer/Lexer.h"

namespace CHTL {

// In ScanToken()
// ...
// case ' ': case '\r': case '\t': break; // Ignore whitespace.
// case '\n': m_Line++; break;
// default: 
//    if (IsDigit(c)) { HandleNumber(); } 
//    else if (IsAlpha(c)) { HandleIdentifier(); } // For keywords and tags
//    else { HandleUnquotedLiteral(); } // Fallback for attribute values etc.
// ...

void Lexer::HandleUnquotedLiteral() {
    while (!IsAtEnd() && !IsWhitespace(Peek()) && Peek() != ';' && Peek() != '}') {
        Advance();
    }
    AddToken(TokenType::String, m_Source.substr(m_Start, m_Current - m_Start));
}

}