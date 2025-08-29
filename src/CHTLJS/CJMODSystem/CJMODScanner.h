#pragma once

#include <string>
#include <vector>

namespace CHTLJS {

enum class CJMODTokenType {
    MODULE,
    IMPORT,
    EXPORT,
    FUNCTION,
    CLASS,
    METADATA,
    ANNOTATION,
    IDENTIFIER,
    STRING,
    NUMBER,
    OPERATOR,
    DELIMITER,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    COLON,
    COMMA,
    COMMENT,
    WHITESPACE,
    EOF_TOKEN
};

struct CJMODToken {
    CJMODTokenType type;
    std::string value;
    int line;
    int column;
};

class CJMODScanner {
public:
    CJMODScanner();
    ~CJMODScanner();
    
    std::vector<CJMODToken> scan(const std::string& code);
    bool isValidSyntax(const std::string& code);
    
private:
    CJMODTokenType determineTokenType(const std::string& value);
    bool validateTokens(const std::vector<CJMODToken>& tokens);
};

} // namespace CHTLJS