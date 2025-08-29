#include "CJMODScanner.h"
#include "../../Error/ErrorReport.h"
#include <regex>

namespace CHTLJS {

CJMODScanner::CJMODScanner() = default;
CJMODScanner::~CJMODScanner() = default;

std::vector<CJMODToken> CJMODScanner::scan(const std::string& code) {
    std::vector<CJMODToken> tokens;
    
    // Simple tokenization for CJMOD syntax
    std::regex tokenRegex(R"((@\w+)|(\w+)|(\{)|(\})|(\[)|(\])|(:)|(,)|("(?:[^"\\]|\\.)*")|('(?:[^'\\]|\\.)*')|(//.*$)|(/\*.*?\*/))");
    
    auto begin = std::sregex_iterator(code.begin(), code.end(), tokenRegex);
    auto end = std::sregex_iterator();
    
    int line = 1;
    int column = 1;
    
    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        std::string tokenValue = match.str();
        
        CJMODToken token;
        token.value = tokenValue;
        token.line = line;
        token.column = column;
        
        // Determine token type
        if (tokenValue[0] == '@') {
            token.type = CJMODTokenType::ANNOTATION;
        } else if (tokenValue == "{") {
            token.type = CJMODTokenType::LBRACE;
        } else if (tokenValue == "}") {
            token.type = CJMODTokenType::RBRACE;
        } else if (tokenValue == "[") {
            token.type = CJMODTokenType::LBRACKET;
        } else if (tokenValue == "]") {
            token.type = CJMODTokenType::RBRACKET;
        } else if (tokenValue == ":") {
            token.type = CJMODTokenType::COLON;
        } else if (tokenValue == ",") {
            token.type = CJMODTokenType::COMMA;
        } else if (tokenValue[0] == '"' || tokenValue[0] == '\'') {
            token.type = CJMODTokenType::STRING;
        } else if (tokenValue.substr(0, 2) == "//" || tokenValue.substr(0, 2) == "/*") {
            token.type = CJMODTokenType::COMMENT;
        } else {
            token.type = CJMODTokenType::IDENTIFIER;
        }
        
        tokens.push_back(token);
        
        // Update position
        column += tokenValue.length();
        size_t newlines = std::count(tokenValue.begin(), tokenValue.end(), '\n');
        if (newlines > 0) {
            line += newlines;
            column = 1;
        }
    }
    
    return tokens;
}

bool CJMODScanner::isValidSyntax(const std::string& code) {
    try {
        auto tokens = scan(code);
        return validateTokens(tokens);
    } catch (const std::exception& e) {
        ErrorBuilder(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR)
            .withMessage("CJMOD syntax validation failed")
            .withDetail(e.what())
            .report();
        return false;
    }
}

bool CJMODScanner::validateTokens(const std::vector<CJMODToken>& tokens) {
    // Basic validation - check for balanced braces
    int braceCount = 0;
    int bracketCount = 0;
    
    for (const auto& token : tokens) {
        switch (token.type) {
            case CJMODTokenType::LBRACE:
                braceCount++;
                break;
            case CJMODTokenType::RBRACE:
                braceCount--;
                if (braceCount < 0) return false;
                break;
            case CJMODTokenType::LBRACKET:
                bracketCount++;
                break;
            case CJMODTokenType::RBRACKET:
                bracketCount--;
                if (bracketCount < 0) return false;
                break;
            default:
                break;
        }
    }
    
    return braceCount == 0 && bracketCount == 0;
}

} // namespace CHTLJS