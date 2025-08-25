// In CHTL_JS/src/Lexer/Lexer.cpp

// This logic will be very similar to the CHTL Lexer's implementation.
void Lexer::HandleUnquotedLiteral() {
    // Consume until a known delimiter is found
    while (!IsAtEnd() && !strchr("{}:,()[]", Peek())) {
        Advance();
    }
    AddToken(TokenType::String, m_Source.substr(m_Start, m_Current - m_Start));
}

// In ScanToken(), add a default case to call HandleUnquotedLiteral()