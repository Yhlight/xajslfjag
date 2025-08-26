#include "CHTLTokens.h"
#include <unordered_map>
#include <unordered_set>

namespace CHTL {

// Token类型名称映射
static const std::unordered_map<CHTLTokenType, std::string> tokenTypeNames = {
    {CHTLTokenType::STRING_LITERAL, "STRING_LITERAL"},
    {CHTLTokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
    {CHTLTokenType::NUMBER, "NUMBER"},
    {CHTLTokenType::IDENTIFIER, "IDENTIFIER"},
    {CHTLTokenType::TEXT, "TEXT"},
    {CHTLTokenType::STYLE, "STYLE"},
    {CHTLTokenType::SCRIPT, "SCRIPT"},
    {CHTLTokenType::USE, "USE"},
    {CHTLTokenType::INHERIT, "INHERIT"},
    {CHTLTokenType::DELETE, "DELETE"},
    {CHTLTokenType::INSERT, "INSERT"},
    {CHTLTokenType::AFTER, "AFTER"},
    {CHTLTokenType::BEFORE, "BEFORE"},
    {CHTLTokenType::REPLACE, "REPLACE"},
    {CHTLTokenType::AT_TOP, "AT_TOP"},
    {CHTLTokenType::AT_BOTTOM, "AT_BOTTOM"},
    {CHTLTokenType::FROM, "FROM"},
    {CHTLTokenType::AS, "AS"},
    {CHTLTokenType::EXCEPT, "EXCEPT"},
    {CHTLTokenType::TEMPLATE, "TEMPLATE"},
    {CHTLTokenType::CUSTOM, "CUSTOM"},
    {CHTLTokenType::ORIGIN, "ORIGIN"},
    {CHTLTokenType::IMPORT, "IMPORT"},
    {CHTLTokenType::NAMESPACE, "NAMESPACE"},
    {CHTLTokenType::CONFIGURATION, "CONFIGURATION"},
    {CHTLTokenType::INFO, "INFO"},
    {CHTLTokenType::EXPORT, "EXPORT"},
    {CHTLTokenType::NAME, "NAME"},
    {CHTLTokenType::ORIGINTYPE, "ORIGINTYPE"},
    {CHTLTokenType::AT_STYLE, "AT_STYLE"},
    {CHTLTokenType::AT_ELEMENT, "AT_ELEMENT"},
    {CHTLTokenType::AT_VAR, "AT_VAR"},
    {CHTLTokenType::AT_HTML, "AT_HTML"},
    {CHTLTokenType::AT_JAVASCRIPT, "AT_JAVASCRIPT"},
    {CHTLTokenType::AT_CHTL, "AT_CHTL"},
    {CHTLTokenType::AT_CJMOD, "AT_CJMOD"},
    {CHTLTokenType::AT_CONFIG, "AT_CONFIG"},
    {CHTLTokenType::LBRACE, "LBRACE"},
    {CHTLTokenType::RBRACE, "RBRACE"},
    {CHTLTokenType::LPAREN, "LPAREN"},
    {CHTLTokenType::RPAREN, "RPAREN"},
    {CHTLTokenType::LBRACKET, "LBRACKET"},
    {CHTLTokenType::RBRACKET, "RBRACKET"},
    {CHTLTokenType::LT, "LT"},
    {CHTLTokenType::GT, "GT"},
    {CHTLTokenType::SEMICOLON, "SEMICOLON"},
    {CHTLTokenType::COLON, "COLON"},
    {CHTLTokenType::EQUAL, "EQUAL"},
    {CHTLTokenType::COMMA, "COMMA"},
    {CHTLTokenType::DOT, "DOT"},
    {CHTLTokenType::SLASH, "SLASH"},
    {CHTLTokenType::STAR, "STAR"},
    {CHTLTokenType::AMPERSAND, "AMPERSAND"},
    {CHTLTokenType::HASH, "HASH"},
    {CHTLTokenType::CLASS_SELECTOR, "CLASS_SELECTOR"},
    {CHTLTokenType::ID_SELECTOR, "ID_SELECTOR"},
    {CHTLTokenType::LINE_COMMENT, "LINE_COMMENT"},
    {CHTLTokenType::BLOCK_COMMENT, "BLOCK_COMMENT"},
    {CHTLTokenType::GENERATOR_COMMENT, "GENERATOR_COMMENT"},
    {CHTLTokenType::NEWLINE, "NEWLINE"},
    {CHTLTokenType::WHITESPACE, "WHITESPACE"},
    {CHTLTokenType::EOF_TOKEN, "EOF_TOKEN"},
    {CHTLTokenType::INVALID, "INVALID"}
};

// CHTL关键字映射
static const std::unordered_map<std::string, CHTLTokenType> keywordMap = {
    // 核心关键字
    {"text", CHTLTokenType::TEXT},
    {"style", CHTLTokenType::STYLE},
    {"script", CHTLTokenType::SCRIPT},
    {"use", CHTLTokenType::USE},
    
    // 声明关键字
    {"template", CHTLTokenType::TEMPLATE},
    {"custom", CHTLTokenType::CUSTOM},
    {"origin", CHTLTokenType::ORIGIN},
    {"import", CHTLTokenType::IMPORT},
    {"namespace", CHTLTokenType::NAMESPACE},
    {"configuration", CHTLTokenType::CONFIGURATION},
    
    // 继承和修改关键字
    {"inherit", CHTLTokenType::INHERIT},
    {"delete", CHTLTokenType::DELETE},
    {"insert", CHTLTokenType::INSERT},
    {"after", CHTLTokenType::AFTER},
    {"before", CHTLTokenType::BEFORE},
    {"replace", CHTLTokenType::REPLACE},
    
    // 导入关键字
    {"from", CHTLTokenType::FROM},
    {"as", CHTLTokenType::AS},
    {"except", CHTLTokenType::EXCEPT},
};

// CHTL前缀映射
static const std::unordered_map<std::string, CHTLTokenType> prefixMap = {
    {"[Template]", CHTLTokenType::TEMPLATE},
    {"[Custom]", CHTLTokenType::CUSTOM},
    {"[Origin]", CHTLTokenType::ORIGIN},
    {"[Import]", CHTLTokenType::IMPORT},
    {"[Namespace]", CHTLTokenType::NAMESPACE},
    {"[Configuration]", CHTLTokenType::CONFIGURATION},
    {"[Info]", CHTLTokenType::INFO},
    {"[Export]", CHTLTokenType::EXPORT},
    {"[Name]", CHTLTokenType::NAME},
    {"[OriginType]", CHTLTokenType::ORIGINTYPE},
};

// 类型标识符映射
static const std::unordered_map<std::string, CHTLTokenType> typeIdentifierMap = {
    {"@Style", CHTLTokenType::AT_STYLE},
    {"@style", CHTLTokenType::AT_STYLE},
    {"@Element", CHTLTokenType::AT_ELEMENT},
    {"@element", CHTLTokenType::AT_ELEMENT},
    {"@Var", CHTLTokenType::AT_VAR},
    {"@var", CHTLTokenType::AT_VAR},
    {"@Html", CHTLTokenType::AT_HTML},
    {"@html", CHTLTokenType::AT_HTML},
    {"@JavaScript", CHTLTokenType::AT_JAVASCRIPT},
    {"@javascript", CHTLTokenType::AT_JAVASCRIPT},
    {"@Chtl", CHTLTokenType::AT_CHTL},
    {"@chtl", CHTLTokenType::AT_CHTL},
    {"@CJmod", CHTLTokenType::AT_CJMOD},
    {"@cjmod", CHTLTokenType::AT_CJMOD},
    {"@Config", CHTLTokenType::AT_CONFIG},
    {"@config", CHTLTokenType::AT_CONFIG},
};

// CHTLToken方法实现
std::string CHTLToken::getTypeName() const {
    auto it = tokenTypeNames.find(type);
    return it != tokenTypeNames.end() ? it->second : "UNKNOWN";
}

bool CHTLToken::isKeyword() const {
    return CHTLTokenUtils::isKeyword(value);
}

bool CHTLToken::isPrefix() const {
    return CHTLTokenUtils::isPrefix(value);
}

bool CHTLToken::isTypeIdentifier() const {
    return CHTLTokenUtils::isTypeIdentifier(value);
}

// CHTLTokenUtils方法实现
CHTLTokenType CHTLTokenUtils::stringToTokenType(const std::string& str) {
    // 检查关键字
    auto keywordIt = keywordMap.find(str);
    if (keywordIt != keywordMap.end()) {
        return keywordIt->second;
    }
    
    // 检查前缀
    auto prefixIt = prefixMap.find(str);
    if (prefixIt != prefixMap.end()) {
        return prefixIt->second;
    }
    
    // 检查类型标识符
    auto typeIt = typeIdentifierMap.find(str);
    if (typeIt != typeIdentifierMap.end()) {
        return typeIt->second;
    }
    
    // 检查特殊情况
    if (str == "at top") return CHTLTokenType::AT_TOP;
    if (str == "at bottom") return CHTLTokenType::AT_BOTTOM;
    
    // 默认为标识符
    return CHTLTokenType::IDENTIFIER;
}

bool CHTLTokenUtils::isKeyword(const std::string& str) {
    return keywordMap.find(str) != keywordMap.end() ||
           str == "at top" || str == "at bottom";
}

bool CHTLTokenUtils::isPrefix(const std::string& str) {
    return prefixMap.find(str) != prefixMap.end();
}

bool CHTLTokenUtils::isTypeIdentifier(const std::string& str) {
    return typeIdentifierMap.find(str) != typeIdentifierMap.end();
}

std::vector<std::string> CHTLTokenUtils::getAllKeywords() {
    std::vector<std::string> keywords;
    
    for (const auto& pair : keywordMap) {
        keywords.push_back(pair.first);
    }
    
    keywords.push_back("at top");
    keywords.push_back("at bottom");
    
    return keywords;
}

} // namespace CHTL