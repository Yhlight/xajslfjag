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
    
    // CHTL核心关键字
    {CHTLTokenType::TEXT, "TEXT"},
    {CHTLTokenType::STYLE, "STYLE"},
    {CHTLTokenType::SCRIPT, "SCRIPT"},
    {CHTLTokenType::USE, "USE"},
    
    // CHTL模板和自定义前缀
    {CHTLTokenType::LBRACKET_TEMPLATE, "LBRACKET_TEMPLATE"},
    {CHTLTokenType::LBRACKET_CUSTOM, "LBRACKET_CUSTOM"},
    {CHTLTokenType::LBRACKET_ORIGIN, "LBRACKET_ORIGIN"},
    {CHTLTokenType::LBRACKET_IMPORT, "LBRACKET_IMPORT"},
    {CHTLTokenType::LBRACKET_NAMESPACE, "LBRACKET_NAMESPACE"},
    {CHTLTokenType::LBRACKET_CONFIGURATION, "LBRACKET_CONFIGURATION"},
    {CHTLTokenType::LBRACKET_INFO, "LBRACKET_INFO"},
    {CHTLTokenType::LBRACKET_EXPORT, "LBRACKET_EXPORT"},
    {CHTLTokenType::LBRACKET_NAME, "LBRACKET_NAME"},
    {CHTLTokenType::LBRACKET_ORIGINTYPE, "LBRACKET_ORIGINTYPE"},
    
    // CHTL类型标识符
    {CHTLTokenType::AT_STYLE, "AT_STYLE"},
    {CHTLTokenType::AT_ELEMENT, "AT_ELEMENT"},
    {CHTLTokenType::AT_VAR, "AT_VAR"},
    {CHTLTokenType::AT_HTML, "AT_HTML"},
    {CHTLTokenType::AT_JAVASCRIPT, "AT_JAVASCRIPT"},
    {CHTLTokenType::AT_CHTL, "AT_CHTL"},
    {CHTLTokenType::AT_CJMOD, "AT_CJMOD"},
    {CHTLTokenType::AT_CONFIG, "AT_CONFIG"},
    
    // CHTL继承和修改关键字
    {CHTLTokenType::INHERIT, "INHERIT"},
    {CHTLTokenType::DELETE, "DELETE"},
    {CHTLTokenType::INSERT, "INSERT"},
    {CHTLTokenType::AFTER, "AFTER"},
    {CHTLTokenType::BEFORE, "BEFORE"},
    {CHTLTokenType::REPLACE, "REPLACE"},
    {CHTLTokenType::AT_TOP, "AT_TOP"},
    {CHTLTokenType::AT_BOTTOM, "AT_BOTTOM"},
    
    // CHTL导入关键字
    {CHTLTokenType::FROM, "FROM"},
    {CHTLTokenType::AS, "AS"},
    {CHTLTokenType::EXCEPT, "EXCEPT"},
    
    // CHTL结构符号
    {CHTLTokenType::LBRACE, "LBRACE"},
    {CHTLTokenType::RBRACE, "RBRACE"},
    {CHTLTokenType::LPAREN, "LPAREN"},
    {CHTLTokenType::RPAREN, "RPAREN"},
    {CHTLTokenType::LBRACKET, "LBRACKET"},
    {CHTLTokenType::RBRACKET, "RBRACKET"},
    {CHTLTokenType::SEMICOLON, "SEMICOLON"},
    {CHTLTokenType::COLON, "COLON"},
    {CHTLTokenType::EQUAL, "EQUAL"},
    {CHTLTokenType::COMMA, "COMMA"},
    {CHTLTokenType::DOT, "DOT"},
    {CHTLTokenType::SLASH, "SLASH"},
    {CHTLTokenType::STAR, "STAR"},
    {CHTLTokenType::AMPERSAND, "AMPERSAND"},
    {CHTLTokenType::HASH, "HASH"},
    
    // CHTL选择器
    {CHTLTokenType::CLASS_SELECTOR, "CLASS_SELECTOR"},
    {CHTLTokenType::ID_SELECTOR, "ID_SELECTOR"},
    
    // CHTL注释
    {CHTLTokenType::LINE_COMMENT, "LINE_COMMENT"},
    {CHTLTokenType::BLOCK_COMMENT, "BLOCK_COMMENT"},
    {CHTLTokenType::GENERATOR_COMMENT, "GENERATOR_COMMENT"},
    
    // 原始嵌入内容
    {CHTLTokenType::ORIGIN_CONTENT, "ORIGIN_CONTENT"},
    
    // CHTL JS相关Token
    {CHTLTokenType::ENHANCED_SELECTOR, "ENHANCED_SELECTOR"},
    {CHTLTokenType::VIR, "VIR"},
    {CHTLTokenType::LISTEN, "LISTEN"},
    {CHTLTokenType::DELEGATE, "DELEGATE"},
    {CHTLTokenType::ANIMATE, "ANIMATE"},
    {CHTLTokenType::ARROW, "ARROW"},
    
    // 特殊Token
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
    
    // CHTL JS关键字
    {"vir", CHTLTokenType::VIR},
    {"listen", CHTLTokenType::LISTEN},
    {"delegate", CHTLTokenType::DELEGATE},
    {"animate", CHTLTokenType::ANIMATE},
};

// CHTL方括号关键字映射
static const std::unordered_map<std::string, CHTLTokenType> bracketKeywordMap = {
    {"[Template]", CHTLTokenType::LBRACKET_TEMPLATE},
    {"[Custom]", CHTLTokenType::LBRACKET_CUSTOM},
    {"[Origin]", CHTLTokenType::LBRACKET_ORIGIN},
    {"[Import]", CHTLTokenType::LBRACKET_IMPORT},
    {"[Namespace]", CHTLTokenType::LBRACKET_NAMESPACE},
    {"[Configuration]", CHTLTokenType::LBRACKET_CONFIGURATION},
    {"[Info]", CHTLTokenType::LBRACKET_INFO},
    {"[Export]", CHTLTokenType::LBRACKET_EXPORT},
    {"[Name]", CHTLTokenType::LBRACKET_NAME},
    {"[OriginType]", CHTLTokenType::LBRACKET_ORIGINTYPE},
};

// CHTL类型标识符映射
static const std::unordered_map<std::string, CHTLTokenType> typeIdentifierMap = {
    {"@Style", CHTLTokenType::AT_STYLE},
    {"@Element", CHTLTokenType::AT_ELEMENT},
    {"@Var", CHTLTokenType::AT_VAR},
    {"@Html", CHTLTokenType::AT_HTML},
    {"@JavaScript", CHTLTokenType::AT_JAVASCRIPT},
    {"@Chtl", CHTLTokenType::AT_CHTL},
    {"@CJmod", CHTLTokenType::AT_CJMOD},
    {"@Config", CHTLTokenType::AT_CONFIG},
};

// CHTLToken方法实现
std::string CHTLToken::getTypeName() const {
    auto it = tokenTypeNames.find(type);
    return it != tokenTypeNames.end() ? it->second : "UNKNOWN";
}

bool CHTLToken::isKeyword() const {
    return CHTLTokenUtils::isKeyword(value);
}

bool CHTLToken::isBracketKeyword() const {
    return CHTLTokenUtils::isBracketKeyword(value);
}

bool CHTLToken::isTypeIdentifier() const {
    return CHTLTokenUtils::isTypeIdentifier(value);
}

bool CHTLToken::isComment() const {
    return CHTLTokenUtils::isComment(type);
}

bool CHTLToken::isStructuralSymbol() const {
    return CHTLTokenUtils::isStructuralSymbol(value);
}

bool CHTLToken::isSelector() const {
    return CHTLTokenUtils::isSelector(type);
}

// CHTLTokenUtils方法实现
CHTLTokenType CHTLTokenUtils::stringToTokenType(const std::string& str) {
    // 检查关键字
    auto keywordIt = keywordMap.find(str);
    if (keywordIt != keywordMap.end()) {
        return keywordIt->second;
    }
    
    // 检查方括号关键字
    auto bracketIt = bracketKeywordMap.find(str);
    if (bracketIt != bracketKeywordMap.end()) {
        return bracketIt->second;
    }
    
    // 检查类型标识符
    auto typeIt = typeIdentifierMap.find(str);
    if (typeIt != typeIdentifierMap.end()) {
        return typeIt->second;
    }
    
    // 检查特殊情况
    if (str == "at top") return CHTLTokenType::AT_TOP;
    if (str == "at bottom") return CHTLTokenType::AT_BOTTOM;
    if (str == "->") return CHTLTokenType::ARROW;
    
    // 默认为标识符
    return CHTLTokenType::IDENTIFIER;
}

std::string CHTLTokenUtils::tokenTypeToString(CHTLTokenType type) {
    auto it = tokenTypeNames.find(type);
    return it != tokenTypeNames.end() ? it->second : "UNKNOWN";
}

bool CHTLTokenUtils::isKeyword(const std::string& str) {
    return keywordMap.find(str) != keywordMap.end();
}

bool CHTLTokenUtils::isBracketKeyword(const std::string& str) {
    return bracketKeywordMap.find(str) != bracketKeywordMap.end();
}

bool CHTLTokenUtils::isTypeIdentifier(const std::string& str) {
    return typeIdentifierMap.find(str) != typeIdentifierMap.end();
}

bool CHTLTokenUtils::isStructuralSymbol(const std::string& str) {
    static const std::unordered_set<std::string> structuralSymbols = {
        "{", "}", "(", ")", "[", "]", ";", ":", "=", ",", ".", "/", "*", "&", "#"
    };
    return structuralSymbols.find(str) != structuralSymbols.end();
}

bool CHTLTokenUtils::isComment(CHTLTokenType type) {
    return type == CHTLTokenType::LINE_COMMENT || 
           type == CHTLTokenType::BLOCK_COMMENT || 
           type == CHTLTokenType::GENERATOR_COMMENT;
}

bool CHTLTokenUtils::isSelector(CHTLTokenType type) {
    return type == CHTLTokenType::CLASS_SELECTOR || 
           type == CHTLTokenType::ID_SELECTOR ||
           type == CHTLTokenType::ENHANCED_SELECTOR;
}

const std::unordered_map<std::string, CHTLTokenType>& CHTLTokenUtils::getKeywordMap() {
    return keywordMap;
}

const std::unordered_map<std::string, CHTLTokenType>& CHTLTokenUtils::getBracketKeywordMap() {
    return bracketKeywordMap;
}

const std::unordered_map<std::string, CHTLTokenType>& CHTLTokenUtils::getTypeIdentifierMap() {
    return typeIdentifierMap;
}

const std::unordered_set<std::string>& CHTLTokenUtils::getCHTLKeywords() {
    static std::unordered_set<std::string> allKeywords;
    if (allKeywords.empty()) {
        for (const auto& pair : keywordMap) {
            allKeywords.insert(pair.first);
        }
        for (const auto& pair : bracketKeywordMap) {
            allKeywords.insert(pair.first);
        }
        for (const auto& pair : typeIdentifierMap) {
            allKeywords.insert(pair.first);
        }
    }
    return allKeywords;
}

} // namespace CHTL