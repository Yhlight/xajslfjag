#include "CHTLToken.h"
#include <unordered_map>
#include <unordered_set>

namespace CHTL {
    namespace Core {
        
        CHTLToken::CHTLToken(CHTLTokenType type, const std::string& value, 
                           const CHTLTokenPosition& position)
            : type_(type), value_(value), position_(position) {
        }
        
        bool CHTLToken::IsKeyword() const {
            return type_ >= CHTLTokenType::KW_TEXT && type_ <= CHTLTokenType::KW_HTML5;
        }
        
        bool CHTLToken::IsTypeIdentifier() const {
            return type_ >= CHTLTokenType::TYPE_STYLE && type_ <= CHTLTokenType::TYPE_CONFIG;
        }
        
        bool CHTLToken::IsBlockIdentifier() const {
            return type_ >= CHTLTokenType::TEMPLATE && type_ <= CHTLTokenType::CONFIGURATION;
        }
        
        bool CHTLToken::IsLiteral() const {
            return type_ == CHTLTokenType::STRING_LITERAL || 
                   type_ == CHTLTokenType::UNQUOTED_LITERAL ||
                   type_ == CHTLTokenType::NUMBER;
        }
        
        bool CHTLToken::IsOperator() const {
            return type_ == CHTLTokenType::COLON ||
                   type_ == CHTLTokenType::EQUALS ||
                   type_ == CHTLTokenType::ARROW ||
                   type_ == CHTLTokenType::BIND_EVENT ||
                   type_ == CHTLTokenType::REFERENCE;
        }
        
        bool CHTLToken::IsComment() const {
            return type_ == CHTLTokenType::LINE_COMMENT ||
                   type_ == CHTLTokenType::BLOCK_COMMENT ||
                   type_ == CHTLTokenType::GENERATOR_COMMENT;
        }
        
        bool CHTLToken::IsHTMLElement() const {
            return type_ == CHTLTokenType::HTML_TAG;
        }
        
        std::string CHTLToken::ToString() const {
            return TokenTypeToString(type_) + ": \"" + value_ + "\" at " +
                   std::to_string(position_.line) + ":" + std::to_string(position_.column);
        }
        
        bool CHTLToken::operator==(const CHTLToken& other) const {
            return type_ == other.type_ && value_ == other.value_;
        }
        
        bool CHTLToken::operator!=(const CHTLToken& other) const {
            return !(*this == other);
        }
        
        std::string TokenTypeToString(CHTLTokenType type) {
            static const std::unordered_map<CHTLTokenType, std::string> typeMap = {
                {CHTLTokenType::IDENTIFIER, "IDENTIFIER"},
                {CHTLTokenType::STRING_LITERAL, "STRING_LITERAL"},
                {CHTLTokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
                {CHTLTokenType::NUMBER, "NUMBER"},
                {CHTLTokenType::COLON, "COLON"},
                {CHTLTokenType::SEMICOLON, "SEMICOLON"},
                {CHTLTokenType::COMMA, "COMMA"},
                {CHTLTokenType::DOT, "DOT"},
                {CHTLTokenType::EQUALS, "EQUALS"},
                {CHTLTokenType::LEFT_BRACE, "LEFT_BRACE"},
                {CHTLTokenType::RIGHT_BRACE, "RIGHT_BRACE"},
                {CHTLTokenType::LEFT_BRACKET, "LEFT_BRACKET"},
                {CHTLTokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
                {CHTLTokenType::LEFT_PAREN, "LEFT_PAREN"},
                {CHTLTokenType::RIGHT_PAREN, "RIGHT_PAREN"},
                {CHTLTokenType::LINE_COMMENT, "LINE_COMMENT"},
                {CHTLTokenType::BLOCK_COMMENT, "BLOCK_COMMENT"},
                {CHTLTokenType::GENERATOR_COMMENT, "GENERATOR_COMMENT"},
                {CHTLTokenType::KW_TEXT, "KW_TEXT"},
                {CHTLTokenType::KW_STYLE, "KW_STYLE"},
                {CHTLTokenType::KW_SCRIPT, "KW_SCRIPT"},
                {CHTLTokenType::KW_INHERIT, "KW_INHERIT"},
                {CHTLTokenType::KW_DELETE, "KW_DELETE"},
                {CHTLTokenType::KW_INSERT, "KW_INSERT"},
                {CHTLTokenType::KW_AFTER, "KW_AFTER"},
                {CHTLTokenType::KW_BEFORE, "KW_BEFORE"},
                {CHTLTokenType::KW_REPLACE, "KW_REPLACE"},
                {CHTLTokenType::KW_AT_TOP, "KW_AT_TOP"},
                {CHTLTokenType::KW_AT_BOTTOM, "KW_AT_BOTTOM"},
                {CHTLTokenType::KW_FROM, "KW_FROM"},
                {CHTLTokenType::KW_AS, "KW_AS"},
                {CHTLTokenType::KW_EXCEPT, "KW_EXCEPT"},
                {CHTLTokenType::KW_USE, "KW_USE"},
                {CHTLTokenType::KW_HTML5, "KW_HTML5"},
                {CHTLTokenType::TEMPLATE, "TEMPLATE"},
                {CHTLTokenType::CUSTOM, "CUSTOM"},
                {CHTLTokenType::ORIGIN, "ORIGIN"},
                {CHTLTokenType::IMPORT, "IMPORT"},
                {CHTLTokenType::NAMESPACE, "NAMESPACE"},
                {CHTLTokenType::CONFIGURATION, "CONFIGURATION"},
                {CHTLTokenType::TYPE_STYLE, "TYPE_STYLE"},
                {CHTLTokenType::TYPE_ELEMENT, "TYPE_ELEMENT"},
                {CHTLTokenType::TYPE_VAR, "TYPE_VAR"},
                {CHTLTokenType::TYPE_HTML, "TYPE_HTML"},
                {CHTLTokenType::TYPE_JAVASCRIPT, "TYPE_JAVASCRIPT"},
                {CHTLTokenType::TYPE_CHTL, "TYPE_CHTL"},
                {CHTLTokenType::TYPE_CJMOD, "TYPE_CJMOD"},
                {CHTLTokenType::TYPE_CONFIG, "TYPE_CONFIG"},
                {CHTLTokenType::SELECTOR_START, "SELECTOR_START"},
                {CHTLTokenType::SELECTOR_END, "SELECTOR_END"},
                {CHTLTokenType::REFERENCE, "REFERENCE"},
                {CHTLTokenType::ARROW, "ARROW"},
                {CHTLTokenType::BIND_EVENT, "BIND_EVENT"},
                {CHTLTokenType::HTML_TAG, "HTML_TAG"},
                {CHTLTokenType::ATTRIBUTE_NAME, "ATTRIBUTE_NAME"},
                {CHTLTokenType::NEWLINE, "NEWLINE"},
                {CHTLTokenType::WHITESPACE, "WHITESPACE"},
                {CHTLTokenType::EOF_TOKEN, "EOF_TOKEN"},
                {CHTLTokenType::UNKNOWN, "UNKNOWN"}
            };
            
            auto it = typeMap.find(type);
            return it != typeMap.end() ? it->second : "UNKNOWN_TYPE";
        }
        
        bool IsKeywordString(const std::string& str) {
            static const std::unordered_set<std::string> keywords = {
                "text", "style", "script", "inherit", "delete", "insert",
                "after", "before", "replace", "at", "top", "bottom",
                "from", "as", "except", "use", "html5"
            };
            
            return keywords.find(str) != keywords.end();
        }
        
        CHTLTokenType GetKeywordType(const std::string& str) {
            static const std::unordered_map<std::string, CHTLTokenType> keywordMap = {
                {"text", CHTLTokenType::KW_TEXT},
                {"style", CHTLTokenType::KW_STYLE},
                {"script", CHTLTokenType::KW_SCRIPT},
                {"inherit", CHTLTokenType::KW_INHERIT},
                {"delete", CHTLTokenType::KW_DELETE},
                {"insert", CHTLTokenType::KW_INSERT},
                {"after", CHTLTokenType::KW_AFTER},
                {"before", CHTLTokenType::KW_BEFORE},
                {"replace", CHTLTokenType::KW_REPLACE},
                {"from", CHTLTokenType::KW_FROM},
                {"as", CHTLTokenType::KW_AS},
                {"except", CHTLTokenType::KW_EXCEPT},
                {"use", CHTLTokenType::KW_USE},
                {"html5", CHTLTokenType::KW_HTML5}
            };
            
            auto it = keywordMap.find(str);
            return it != keywordMap.end() ? it->second : CHTLTokenType::IDENTIFIER;
        }
        
    } // namespace Core
} // namespace CHTL