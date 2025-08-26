#include "CHTLJSTokens.h"
#include <algorithm>
#include <regex>

namespace CHTLJS {

// ====== CHTLJSToken 实现 ======

void CHTLJSToken::updateProperties() {
    isKeyword = CHTLJSTokenUtils::isKeyword(value);
    isOperator = CHTLJSTokenUtils::isOperator(value);
    isLiteral = CHTLJSTokenUtils::isLiteral(type);
    isCHTLJSSpecific = CHTLJSTokenUtils::isCHTLJSSpecificToken(type);
}

std::string CHTLJSToken::getTypeName() const {
    return CHTLJSTokenUtils::tokenTypeToString(type);
}

std::string CHTLJSToken::getDescription() const {
    std::string desc = getTypeName();
    if (!value.empty()) {
        desc += " (" + value + ")";
    }
    return desc;
}

bool CHTLJSToken::isComment() const {
    return type == CHTLJSTokenType::LINE_COMMENT ||
           type == CHTLJSTokenType::BLOCK_COMMENT ||
           type == CHTLJSTokenType::JSDOC_COMMENT;
}

bool CHTLJSToken::isSelectorKeyword() const {
    return CHTLJSTokenUtils::isSelectorKeyword(value);
}

bool CHTLJSToken::isListenerKeyword() const {
    return CHTLJSTokenUtils::isListenerKeyword(value);
}

bool CHTLJSToken::isAnimationKeyword() const {
    return CHTLJSTokenUtils::isAnimationKeyword(value);
}

bool CHTLJSToken::isVirtualObjectKeyword() const {
    return CHTLJSTokenUtils::isVirtualObjectKeyword(value);
}

bool CHTLJSToken::isEventType() const {
    return CHTLJSTokenUtils::isEventType(value);
}

bool CHTLJSToken::isLifecycleKeyword() const {
    const auto& lifecycleKeywords = CHTLJSTokenUtils::getVirtualObjectKeywords();
    return std::find(lifecycleKeywords.begin(), lifecycleKeywords.end(), value) != lifecycleKeywords.end();
}

// ====== CHTLJSTokenUtils 实现 ======

const std::unordered_map<std::string, CHTLJSTokenType>& CHTLJSTokenUtils::getKeywordMap() {
    static std::unordered_map<std::string, CHTLJSTokenType> keywordMap = {
        // JavaScript基础关键字
        {"const", CHTLJSTokenType::CONST},
        {"let", CHTLJSTokenType::LET},
        {"var", CHTLJSTokenType::VAR},
        {"function", CHTLJSTokenType::FUNCTION},
        {"async", CHTLJSTokenType::ASYNC},
        {"await", CHTLJSTokenType::AWAIT},
        {"class", CHTLJSTokenType::CLASS},
        {"extends", CHTLJSTokenType::EXTENDS},
        {"super", CHTLJSTokenType::SUPER},
        {"if", CHTLJSTokenType::IF},
        {"else", CHTLJSTokenType::ELSE},
        {"switch", CHTLJSTokenType::SWITCH},
        {"case", CHTLJSTokenType::CASE},
        {"default", CHTLJSTokenType::DEFAULT},
        {"for", CHTLJSTokenType::FOR},
        {"while", CHTLJSTokenType::WHILE},
        {"do", CHTLJSTokenType::DO},
        {"break", CHTLJSTokenType::BREAK},
        {"continue", CHTLJSTokenType::CONTINUE},
        {"try", CHTLJSTokenType::TRY},
        {"catch", CHTLJSTokenType::CATCH},
        {"finally", CHTLJSTokenType::FINALLY},
        {"throw", CHTLJSTokenType::THROW},
        {"return", CHTLJSTokenType::RETURN},
        {"yield", CHTLJSTokenType::YIELD},
        {"import", CHTLJSTokenType::IMPORT},
        {"export", CHTLJSTokenType::EXPORT},
        {"from", CHTLJSTokenType::FROM},
        {"as", CHTLJSTokenType::AS},
        {"new", CHTLJSTokenType::NEW},
        {"delete", CHTLJSTokenType::DELETE},
        {"typeof", CHTLJSTokenType::TYPEOF},
        {"instanceof", CHTLJSTokenType::INSTANCEOF},
        {"in", CHTLJSTokenType::IN},
        {"of", CHTLJSTokenType::OF},
        {"this", CHTLJSTokenType::THIS},
        {"true", CHTLJSTokenType::BOOLEAN},
        {"false", CHTLJSTokenType::BOOLEAN},
        {"null", CHTLJSTokenType::NULL_LITERAL},
        {"undefined", CHTLJSTokenType::UNDEFINED_LITERAL},
        
        // CHTL JS增强选择器关键字
        {"selector", CHTLJSTokenType::SELECTOR},
        {"select", CHTLJSTokenType::SELECT},
        {"query", CHTLJSTokenType::QUERY},
        {"queryAll", CHTLJSTokenType::QUERYALL},
        {"closest", CHTLJSTokenType::CLOSEST},
        {"parent", CHTLJSTokenType::PARENT},
        {"child", CHTLJSTokenType::CHILD},
        {"siblings", CHTLJSTokenType::SIBLINGS},
        {"next", CHTLJSTokenType::NEXT},
        {"prev", CHTLJSTokenType::PREV},
        {"find", CHTLJSTokenType::FIND},
        {"filter", CHTLJSTokenType::FILTER},
        {"has", CHTLJSTokenType::HAS},
        {"first", CHTLJSTokenType::FIRST},
        {"last", CHTLJSTokenType::LAST},
        {"index", CHTLJSTokenType::INDEX},
        {"visible", CHTLJSTokenType::VISIBLE},
        {"hidden", CHTLJSTokenType::HIDDEN},
        {"enabled", CHTLJSTokenType::ENABLED},
        {"disabled", CHTLJSTokenType::DISABLED},
        {"checked", CHTLJSTokenType::CHECKED},
        {"selected", CHTLJSTokenType::SELECTED},
        {"empty", CHTLJSTokenType::EMPTY},
        {"contains", CHTLJSTokenType::CONTAINS},
        
        // CHTL JS监听器增强关键字
        {"listener", CHTLJSTokenType::LISTENER},
        {"listen", CHTLJSTokenType::LISTEN},
        {"on", CHTLJSTokenType::ON},
        {"event", CHTLJSTokenType::EVENT},
        {"trigger", CHTLJSTokenType::TRIGGER},
        {"click", CHTLJSTokenType::CLICK},
        {"dblclick", CHTLJSTokenType::DBLCLICK},
        {"mousedown", CHTLJSTokenType::MOUSEDOWN},
        {"mouseup", CHTLJSTokenType::MOUSEUP},
        {"mouseover", CHTLJSTokenType::MOUSEOVER},
        {"mouseout", CHTLJSTokenType::MOUSEOUT},
        {"mousemove", CHTLJSTokenType::MOUSEMOVE},
        {"keydown", CHTLJSTokenType::KEYDOWN},
        {"keyup", CHTLJSTokenType::KEYUP},
        {"keypress", CHTLJSTokenType::KEYPRESS},
        {"focus", CHTLJSTokenType::FOCUS},
        {"blur", CHTLJSTokenType::BLUR},
        {"change", CHTLJSTokenType::CHANGE},
        {"input", CHTLJSTokenType::INPUT},
        {"submit", CHTLJSTokenType::SUBMIT},
        {"reset", CHTLJSTokenType::RESET},
        {"load", CHTLJSTokenType::LOAD},
        {"unload", CHTLJSTokenType::UNLOAD},
        {"resize", CHTLJSTokenType::RESIZE},
        {"scroll", CHTLJSTokenType::SCROLL},
        {"drag", CHTLJSTokenType::DRAG},
        {"drop", CHTLJSTokenType::DROP},
        {"dragover", CHTLJSTokenType::DRAGOVER},
        {"touch", CHTLJSTokenType::TOUCH},
        {"touchstart", CHTLJSTokenType::TOUCHSTART},
        {"touchend", CHTLJSTokenType::TOUCHEND},
        
        // CHTL JS事件委托关键字
        {"delegate", CHTLJSTokenType::DELEGATE},
        {"delegated", CHTLJSTokenType::DELEGATED},
        {"bubble", CHTLJSTokenType::BUBBLE},
        {"capture", CHTLJSTokenType::CAPTURE},
        {"prevent", CHTLJSTokenType::PREVENT},
        {"stop", CHTLJSTokenType::STOP},
        {"once", CHTLJSTokenType::ONCE},
        {"passive", CHTLJSTokenType::PASSIVE},
        
        // CHTL JS动画关键字
        {"animate", CHTLJSTokenType::ANIMATE},
        {"animation", CHTLJSTokenType::ANIMATION},
        {"transition", CHTLJSTokenType::TRANSITION},
        {"transform", CHTLJSTokenType::TRANSFORM},
        {"keyframes", CHTLJSTokenType::KEYFRAMES},
        {"frame", CHTLJSTokenType::FRAME},
        {"duration", CHTLJSTokenType::DURATION},
        {"delay", CHTLJSTokenType::DELAY},
        {"easing", CHTLJSTokenType::EASING},
        {"timing", CHTLJSTokenType::TIMING},
        {"infinite", CHTLJSTokenType::INFINITE},
        {"alternate", CHTLJSTokenType::ALTERNATE},
        {"forwards", CHTLJSTokenType::FORWARDS},
        {"backwards", CHTLJSTokenType::BACKWARDS},
        {"play", CHTLJSTokenType::PLAY},
        {"pause", CHTLJSTokenType::PAUSE},
        {"stop", CHTLJSTokenType::STOP_ANIM},
        {"fade", CHTLJSTokenType::FADE},
        {"slide", CHTLJSTokenType::SLIDE},
        {"scale", CHTLJSTokenType::SCALE},
        {"rotate", CHTLJSTokenType::ROTATE},
        {"translate", CHTLJSTokenType::TRANSLATE},
        {"skew", CHTLJSTokenType::SKEW},
        
        // CHTL JS虚拟对象关键字
        {"virtual", CHTLJSTokenType::VIRTUAL},
        {"vnode", CHTLJSTokenType::VNODE},
        {"component", CHTLJSTokenType::COMPONENT},
        {"mount", CHTLJSTokenType::MOUNT},
        {"unmount", CHTLJSTokenType::UNMOUNT},
        {"render", CHTLJSTokenType::RENDER},
        {"update", CHTLJSTokenType::UPDATE},
        {"state", CHTLJSTokenType::STATE},
        {"props", CHTLJSTokenType::PROPS},
        {"computed", CHTLJSTokenType::COMPUTED},
        {"watch", CHTLJSTokenType::WATCH},
        {"reactive", CHTLJSTokenType::REACTIVE},
        {"ref", CHTLJSTokenType::REF},
        {"refs", CHTLJSTokenType::REFS},
        {"emit", CHTLJSTokenType::EMIT},
        {"dispatch", CHTLJSTokenType::DISPATCH},
        {"lifecycle", CHTLJSTokenType::LIFECYCLE},
        {"hook", CHTLJSTokenType::HOOK},
        {"created", CHTLJSTokenType::CREATED},
        {"mounted", CHTLJSTokenType::MOUNTED},
        {"updated", CHTLJSTokenType::UPDATED},
        {"destroyed", CHTLJSTokenType::DESTROYED}
    };
    return keywordMap;
}

const std::unordered_map<std::string, CHTLJSTokenType>& CHTLJSTokenUtils::getOperatorMap() {
    static std::unordered_map<std::string, CHTLJSTokenType> operatorMap = {
        // 算术操作符
        {"+", CHTLJSTokenType::PLUS},
        {"-", CHTLJSTokenType::MINUS},
        {"*", CHTLJSTokenType::MULTIPLY},
        {"/", CHTLJSTokenType::DIVIDE},
        {"%", CHTLJSTokenType::MODULO},
        {"**", CHTLJSTokenType::EXPONENT},
        
        // 赋值操作符
        {"=", CHTLJSTokenType::ASSIGN},
        {"+=", CHTLJSTokenType::PLUS_ASSIGN},
        {"-=", CHTLJSTokenType::MINUS_ASSIGN},
        {"*=", CHTLJSTokenType::MULTIPLY_ASSIGN},
        {"/=", CHTLJSTokenType::DIVIDE_ASSIGN},
        {"%=", CHTLJSTokenType::MODULO_ASSIGN},
        
        // 比较操作符
        {"==", CHTLJSTokenType::EQUAL},
        {"!=", CHTLJSTokenType::NOT_EQUAL},
        {"===", CHTLJSTokenType::STRICT_EQUAL},
        {"!==", CHTLJSTokenType::STRICT_NOT_EQUAL},
        {"<", CHTLJSTokenType::LESS_THAN},
        {">", CHTLJSTokenType::GREATER_THAN},
        {"<=", CHTLJSTokenType::LESS_EQUAL},
        {">=", CHTLJSTokenType::GREATER_EQUAL},
        
        // 逻辑操作符
        {"&&", CHTLJSTokenType::LOGICAL_AND},
        {"||", CHTLJSTokenType::LOGICAL_OR},
        {"!", CHTLJSTokenType::LOGICAL_NOT},
        
        // 位操作符
        {"&", CHTLJSTokenType::BITWISE_AND},
        {"|", CHTLJSTokenType::BITWISE_OR},
        {"^", CHTLJSTokenType::BITWISE_XOR},
        {"~", CHTLJSTokenType::BITWISE_NOT},
        {"<<", CHTLJSTokenType::LEFT_SHIFT},
        {">>", CHTLJSTokenType::RIGHT_SHIFT},
        {">>>", CHTLJSTokenType::UNSIGNED_RIGHT_SHIFT},
        
        // 递增递减
        {"++", CHTLJSTokenType::INCREMENT},
        {"--", CHTLJSTokenType::DECREMENT},
        
        // CHTL JS增强操作符
        {"::", CHTLJSTokenType::BIND_OP},
        {"|>", CHTLJSTokenType::PIPE_OP},
        {"?.", CHTLJSTokenType::CHAIN_OP},
        {"??", CHTLJSTokenType::NULL_COALESCE},
        {"..", CHTLJSTokenType::RANGE_OP},
        {"...", CHTLJSTokenType::SPREAD_OP},
        
        // 箭头函数
        {"=>", CHTLJSTokenType::ARROW}
    };
    return operatorMap;
}

const std::unordered_map<CHTLJSTokenType, std::string>& CHTLJSTokenUtils::getTokenNameMap() {
    static std::unordered_map<CHTLJSTokenType, std::string> tokenNameMap = {
        // 基础字面量
        {CHTLJSTokenType::STRING_LITERAL, "STRING_LITERAL"},
        {CHTLJSTokenType::TEMPLATE_LITERAL, "TEMPLATE_LITERAL"},
        {CHTLJSTokenType::NUMBER, "NUMBER"},
        {CHTLJSTokenType::BIGINT, "BIGINT"},
        {CHTLJSTokenType::BOOLEAN, "BOOLEAN"},
        {CHTLJSTokenType::NULL_LITERAL, "NULL_LITERAL"},
        {CHTLJSTokenType::UNDEFINED_LITERAL, "UNDEFINED_LITERAL"},
        {CHTLJSTokenType::IDENTIFIER, "IDENTIFIER"},
        
        // JavaScript关键字
        {CHTLJSTokenType::CONST, "CONST"},
        {CHTLJSTokenType::LET, "LET"},
        {CHTLJSTokenType::VAR, "VAR"},
        {CHTLJSTokenType::FUNCTION, "FUNCTION"},
        {CHTLJSTokenType::ASYNC, "ASYNC"},
        {CHTLJSTokenType::AWAIT, "AWAIT"},
        {CHTLJSTokenType::CLASS, "CLASS"},
        {CHTLJSTokenType::EXTENDS, "EXTENDS"},
        {CHTLJSTokenType::SUPER, "SUPER"},
        {CHTLJSTokenType::IF, "IF"},
        {CHTLJSTokenType::ELSE, "ELSE"},
        {CHTLJSTokenType::SWITCH, "SWITCH"},
        {CHTLJSTokenType::CASE, "CASE"},
        {CHTLJSTokenType::DEFAULT, "DEFAULT"},
        {CHTLJSTokenType::FOR, "FOR"},
        {CHTLJSTokenType::WHILE, "WHILE"},
        {CHTLJSTokenType::DO, "DO"},
        {CHTLJSTokenType::BREAK, "BREAK"},
        {CHTLJSTokenType::CONTINUE, "CONTINUE"},
        {CHTLJSTokenType::TRY, "TRY"},
        {CHTLJSTokenType::CATCH, "CATCH"},
        {CHTLJSTokenType::FINALLY, "FINALLY"},
        {CHTLJSTokenType::THROW, "THROW"},
        {CHTLJSTokenType::RETURN, "RETURN"},
        {CHTLJSTokenType::YIELD, "YIELD"},
        {CHTLJSTokenType::IMPORT, "IMPORT"},
        {CHTLJSTokenType::EXPORT, "EXPORT"},
        {CHTLJSTokenType::FROM, "FROM"},
        {CHTLJSTokenType::AS, "AS"},
        {CHTLJSTokenType::NEW, "NEW"},
        {CHTLJSTokenType::DELETE, "DELETE"},
        {CHTLJSTokenType::TYPEOF, "TYPEOF"},
        {CHTLJSTokenType::INSTANCEOF, "INSTANCEOF"},
        {CHTLJSTokenType::IN, "IN"},
        {CHTLJSTokenType::OF, "OF"},
        {CHTLJSTokenType::THIS, "THIS"},
        
        // CHTL JS特性
        {CHTLJSTokenType::SELECTOR, "SELECTOR"},
        {CHTLJSTokenType::LISTENER, "LISTENER"},
        {CHTLJSTokenType::ANIMATE, "ANIMATE"},
        {CHTLJSTokenType::VIRTUAL, "VIRTUAL"},
        
        // 操作符
        {CHTLJSTokenType::PLUS, "PLUS"},
        {CHTLJSTokenType::MINUS, "MINUS"},
        {CHTLJSTokenType::MULTIPLY, "MULTIPLY"},
        {CHTLJSTokenType::DIVIDE, "DIVIDE"},
        {CHTLJSTokenType::MODULO, "MODULO"},
        {CHTLJSTokenType::ASSIGN, "ASSIGN"},
        {CHTLJSTokenType::EQUAL, "EQUAL"},
        {CHTLJSTokenType::NOT_EQUAL, "NOT_EQUAL"},
        {CHTLJSTokenType::LESS_THAN, "LESS_THAN"},
        {CHTLJSTokenType::GREATER_THAN, "GREATER_THAN"},
        {CHTLJSTokenType::LOGICAL_AND, "LOGICAL_AND"},
        {CHTLJSTokenType::LOGICAL_OR, "LOGICAL_OR"},
        {CHTLJSTokenType::LOGICAL_NOT, "LOGICAL_NOT"},
        
        // 结构符号
        {CHTLJSTokenType::LBRACE, "LBRACE"},
        {CHTLJSTokenType::RBRACE, "RBRACE"},
        {CHTLJSTokenType::LPAREN, "LPAREN"},
        {CHTLJSTokenType::RPAREN, "RPAREN"},
        {CHTLJSTokenType::LBRACKET, "LBRACKET"},
        {CHTLJSTokenType::RBRACKET, "RBRACKET"},
        {CHTLJSTokenType::SEMICOLON, "SEMICOLON"},
        {CHTLJSTokenType::COLON, "COLON"},
        {CHTLJSTokenType::COMMA, "COMMA"},
        {CHTLJSTokenType::DOT, "DOT"},
        {CHTLJSTokenType::ARROW, "ARROW"},
        
        // 特殊Token
        {CHTLJSTokenType::NEWLINE, "NEWLINE"},
        {CHTLJSTokenType::WHITESPACE, "WHITESPACE"},
        {CHTLJSTokenType::EOF_TOKEN, "EOF_TOKEN"},
        {CHTLJSTokenType::INVALID, "INVALID"}
    };
    return tokenNameMap;
}

CHTLJSTokenType CHTLJSTokenUtils::stringToTokenType(const std::string& str) {
    const auto& keywordMap = getKeywordMap();
    auto it = keywordMap.find(str);
    if (it != keywordMap.end()) {
        return it->second;
    }
    
    const auto& operatorMap = getOperatorMap();
    auto opIt = operatorMap.find(str);
    if (opIt != operatorMap.end()) {
        return opIt->second;
    }
    
    return CHTLJSTokenType::IDENTIFIER;
}

std::string CHTLJSTokenUtils::tokenTypeToString(CHTLJSTokenType type) {
    const auto& tokenNameMap = getTokenNameMap();
    auto it = tokenNameMap.find(type);
    return it != tokenNameMap.end() ? it->second : "UNKNOWN";
}

bool CHTLJSTokenUtils::isKeyword(const std::string& str) {
    const auto& keywordMap = getKeywordMap();
    return keywordMap.find(str) != keywordMap.end();
}

bool CHTLJSTokenUtils::isJavaScriptKeyword(const std::string& str) {
    const auto& jsKeywords = getJavaScriptKeywordSet();
    CHTLJSTokenType type = stringToTokenType(str);
    return jsKeywords.find(type) != jsKeywords.end();
}

bool CHTLJSTokenUtils::isCHTLJSKeyword(const std::string& str) {
    const auto& chtljsKeywords = getCHTLJSKeywordSet();
    CHTLJSTokenType type = stringToTokenType(str);
    return chtljsKeywords.find(type) != chtljsKeywords.end();
}

bool CHTLJSTokenUtils::isSelectorKeyword(const std::string& str) {
    const auto& selectorKeywords = getSelectorKeywordSet();
    CHTLJSTokenType type = stringToTokenType(str);
    return selectorKeywords.find(type) != selectorKeywords.end();
}

bool CHTLJSTokenUtils::isListenerKeyword(const std::string& str) {
    const auto& listenerKeywords = getListenerKeywordSet();
    CHTLJSTokenType type = stringToTokenType(str);
    return listenerKeywords.find(type) != listenerKeywords.end();
}

bool CHTLJSTokenUtils::isAnimationKeyword(const std::string& str) {
    const auto& animationKeywords = getAnimationKeywordSet();
    CHTLJSTokenType type = stringToTokenType(str);
    return animationKeywords.find(type) != animationKeywords.end();
}

bool CHTLJSTokenUtils::isVirtualObjectKeyword(const std::string& str) {
    const auto& virtualKeywords = getVirtualObjectKeywordSet();
    CHTLJSTokenType type = stringToTokenType(str);
    return virtualKeywords.find(type) != virtualKeywords.end();
}

bool CHTLJSTokenUtils::isEventType(const std::string& str) {
    const auto& eventTypes = getEventTypeSet();
    CHTLJSTokenType type = stringToTokenType(str);
    return eventTypes.find(type) != eventTypes.end();
}

bool CHTLJSTokenUtils::isOperator(const std::string& str) {
    const auto& operatorMap = getOperatorMap();
    return operatorMap.find(str) != operatorMap.end();
}

bool CHTLJSTokenUtils::isLiteral(CHTLJSTokenType type) {
    const auto& literalSet = getLiteralSet();
    return literalSet.find(type) != literalSet.end();
}

bool CHTLJSTokenUtils::isCHTLJSSpecificToken(CHTLJSTokenType type) {
    const auto& chtljsSpecific = getCHTLJSSpecificSet();
    return chtljsSpecific.find(type) != chtljsSpecific.end();
}

// 获取分类集合
const std::unordered_set<CHTLJSTokenType>& CHTLJSTokenUtils::getJavaScriptKeywordSet() {
    static std::unordered_set<CHTLJSTokenType> jsKeywords = {
        CHTLJSTokenType::CONST, CHTLJSTokenType::LET, CHTLJSTokenType::VAR,
        CHTLJSTokenType::FUNCTION, CHTLJSTokenType::ASYNC, CHTLJSTokenType::AWAIT,
        CHTLJSTokenType::CLASS, CHTLJSTokenType::EXTENDS, CHTLJSTokenType::SUPER,
        CHTLJSTokenType::IF, CHTLJSTokenType::ELSE, CHTLJSTokenType::SWITCH,
        CHTLJSTokenType::CASE, CHTLJSTokenType::DEFAULT, CHTLJSTokenType::FOR,
        CHTLJSTokenType::WHILE, CHTLJSTokenType::DO, CHTLJSTokenType::BREAK,
        CHTLJSTokenType::CONTINUE, CHTLJSTokenType::TRY, CHTLJSTokenType::CATCH,
        CHTLJSTokenType::FINALLY, CHTLJSTokenType::THROW, CHTLJSTokenType::RETURN,
        CHTLJSTokenType::YIELD, CHTLJSTokenType::IMPORT, CHTLJSTokenType::EXPORT,
        CHTLJSTokenType::FROM, CHTLJSTokenType::AS, CHTLJSTokenType::NEW,
        CHTLJSTokenType::DELETE, CHTLJSTokenType::TYPEOF, CHTLJSTokenType::INSTANCEOF,
        CHTLJSTokenType::IN, CHTLJSTokenType::OF, CHTLJSTokenType::THIS
    };
    return jsKeywords;
}

const std::unordered_set<CHTLJSTokenType>& CHTLJSTokenUtils::getSelectorKeywordSet() {
    static std::unordered_set<CHTLJSTokenType> selectorKeywords = {
        CHTLJSTokenType::SELECTOR, CHTLJSTokenType::SELECT, CHTLJSTokenType::QUERY,
        CHTLJSTokenType::QUERYALL, CHTLJSTokenType::CLOSEST, CHTLJSTokenType::PARENT,
        CHTLJSTokenType::CHILD, CHTLJSTokenType::SIBLINGS, CHTLJSTokenType::NEXT,
        CHTLJSTokenType::PREV, CHTLJSTokenType::FIND, CHTLJSTokenType::FILTER,
        CHTLJSTokenType::HAS, CHTLJSTokenType::FIRST, CHTLJSTokenType::LAST,
        CHTLJSTokenType::INDEX, CHTLJSTokenType::VISIBLE, CHTLJSTokenType::HIDDEN,
        CHTLJSTokenType::ENABLED, CHTLJSTokenType::DISABLED, CHTLJSTokenType::CHECKED,
        CHTLJSTokenType::SELECTED, CHTLJSTokenType::EMPTY, CHTLJSTokenType::CONTAINS
    };
    return selectorKeywords;
}

const std::unordered_set<CHTLJSTokenType>& CHTLJSTokenUtils::getListenerKeywordSet() {
    static std::unordered_set<CHTLJSTokenType> listenerKeywords = {
        CHTLJSTokenType::LISTENER, CHTLJSTokenType::LISTEN, CHTLJSTokenType::ON,
        CHTLJSTokenType::EVENT, CHTLJSTokenType::TRIGGER, CHTLJSTokenType::DELEGATE,
        CHTLJSTokenType::DELEGATED, CHTLJSTokenType::BUBBLE, CHTLJSTokenType::CAPTURE,
        CHTLJSTokenType::PREVENT, CHTLJSTokenType::STOP, CHTLJSTokenType::ONCE,
        CHTLJSTokenType::PASSIVE
    };
    return listenerKeywords;
}

const std::unordered_set<CHTLJSTokenType>& CHTLJSTokenUtils::getEventTypeSet() {
    static std::unordered_set<CHTLJSTokenType> eventTypes = {
        CHTLJSTokenType::CLICK, CHTLJSTokenType::DBLCLICK, CHTLJSTokenType::MOUSEDOWN,
        CHTLJSTokenType::MOUSEUP, CHTLJSTokenType::MOUSEOVER, CHTLJSTokenType::MOUSEOUT,
        CHTLJSTokenType::MOUSEMOVE, CHTLJSTokenType::KEYDOWN, CHTLJSTokenType::KEYUP,
        CHTLJSTokenType::KEYPRESS, CHTLJSTokenType::FOCUS, CHTLJSTokenType::BLUR,
        CHTLJSTokenType::CHANGE, CHTLJSTokenType::INPUT, CHTLJSTokenType::SUBMIT,
        CHTLJSTokenType::RESET, CHTLJSTokenType::LOAD, CHTLJSTokenType::UNLOAD,
        CHTLJSTokenType::RESIZE, CHTLJSTokenType::SCROLL, CHTLJSTokenType::DRAG,
        CHTLJSTokenType::DROP, CHTLJSTokenType::DRAGOVER, CHTLJSTokenType::TOUCH,
        CHTLJSTokenType::TOUCHSTART, CHTLJSTokenType::TOUCHEND
    };
    return eventTypes;
}

const std::unordered_set<CHTLJSTokenType>& CHTLJSTokenUtils::getAnimationKeywordSet() {
    static std::unordered_set<CHTLJSTokenType> animationKeywords = {
        CHTLJSTokenType::ANIMATE, CHTLJSTokenType::ANIMATION, CHTLJSTokenType::TRANSITION,
        CHTLJSTokenType::TRANSFORM, CHTLJSTokenType::KEYFRAMES, CHTLJSTokenType::FRAME,
        CHTLJSTokenType::DURATION, CHTLJSTokenType::DELAY, CHTLJSTokenType::EASING,
        CHTLJSTokenType::TIMING, CHTLJSTokenType::INFINITE, CHTLJSTokenType::ALTERNATE,
        CHTLJSTokenType::FORWARDS, CHTLJSTokenType::BACKWARDS, CHTLJSTokenType::PLAY,
        CHTLJSTokenType::PAUSE, CHTLJSTokenType::STOP_ANIM, CHTLJSTokenType::FADE,
        CHTLJSTokenType::SLIDE, CHTLJSTokenType::SCALE, CHTLJSTokenType::ROTATE,
        CHTLJSTokenType::TRANSLATE, CHTLJSTokenType::SKEW
    };
    return animationKeywords;
}

const std::unordered_set<CHTLJSTokenType>& CHTLJSTokenUtils::getVirtualObjectKeywordSet() {
    static std::unordered_set<CHTLJSTokenType> virtualKeywords = {
        CHTLJSTokenType::VIRTUAL, CHTLJSTokenType::VNODE, CHTLJSTokenType::COMPONENT,
        CHTLJSTokenType::MOUNT, CHTLJSTokenType::UNMOUNT, CHTLJSTokenType::RENDER,
        CHTLJSTokenType::UPDATE, CHTLJSTokenType::STATE, CHTLJSTokenType::PROPS,
        CHTLJSTokenType::COMPUTED, CHTLJSTokenType::WATCH, CHTLJSTokenType::REACTIVE,
        CHTLJSTokenType::REF, CHTLJSTokenType::REFS, CHTLJSTokenType::EMIT,
        CHTLJSTokenType::DISPATCH, CHTLJSTokenType::LIFECYCLE, CHTLJSTokenType::HOOK,
        CHTLJSTokenType::CREATED, CHTLJSTokenType::MOUNTED, CHTLJSTokenType::UPDATED,
        CHTLJSTokenType::DESTROYED
    };
    return virtualKeywords;
}

const std::unordered_set<CHTLJSTokenType>& CHTLJSTokenUtils::getLiteralSet() {
    static std::unordered_set<CHTLJSTokenType> literals = {
        CHTLJSTokenType::STRING_LITERAL, CHTLJSTokenType::TEMPLATE_LITERAL,
        CHTLJSTokenType::NUMBER, CHTLJSTokenType::BIGINT, CHTLJSTokenType::BOOLEAN,
        CHTLJSTokenType::NULL_LITERAL, CHTLJSTokenType::UNDEFINED_LITERAL,
        CHTLJSTokenType::REGEX_LITERAL
    };
    return literals;
}

const std::unordered_set<CHTLJSTokenType>& CHTLJSTokenUtils::getCHTLJSSpecificSet() {
    static std::unordered_set<CHTLJSTokenType> chtljsSpecific;
    
    // 合并所有CHTL JS特有的关键字集合
    const auto& selectorSet = getSelectorKeywordSet();
    const auto& listenerSet = getListenerKeywordSet();
    const auto& animationSet = getAnimationKeywordSet();
    const auto& virtualSet = getVirtualObjectKeywordSet();
    const auto& eventSet = getEventTypeSet();
    
    chtljsSpecific.insert(selectorSet.begin(), selectorSet.end());
    chtljsSpecific.insert(listenerSet.begin(), listenerSet.end());
    chtljsSpecific.insert(animationSet.begin(), animationSet.end());
    chtljsSpecific.insert(virtualSet.begin(), virtualSet.end());
    chtljsSpecific.insert(eventSet.begin(), eventSet.end());
    
    // 添加增强操作符
    chtljsSpecific.insert({
        CHTLJSTokenType::BIND_OP, CHTLJSTokenType::PIPE_OP,
        CHTLJSTokenType::CHAIN_OP, CHTLJSTokenType::NULL_COALESCE,
        CHTLJSTokenType::RANGE_OP
    });
    
    return chtljsSpecific;
}

// 获取关键字列表
std::vector<std::string> CHTLJSTokenUtils::getAllKeywords() {
    std::vector<std::string> keywords;
    const auto& keywordMap = getKeywordMap();
    for (const auto& pair : keywordMap) {
        keywords.push_back(pair.first);
    }
    std::sort(keywords.begin(), keywords.end());
    return keywords;
}

std::vector<std::string> CHTLJSTokenUtils::getSelectorKeywords() {
    return {"selector", "select", "query", "queryAll", "closest", "parent", "child",
            "siblings", "next", "prev", "find", "filter", "has", "first", "last",
            "index", "visible", "hidden", "enabled", "disabled", "checked",
            "selected", "empty", "contains"};
}

std::vector<std::string> CHTLJSTokenUtils::getListenerKeywords() {
    return {"listener", "listen", "on", "event", "trigger", "delegate", "delegated",
            "bubble", "capture", "prevent", "stop", "once", "passive"};
}

std::vector<std::string> CHTLJSTokenUtils::getAnimationKeywords() {
    return {"animate", "animation", "transition", "transform", "keyframes", "frame",
            "duration", "delay", "easing", "timing", "infinite", "alternate",
            "forwards", "backwards", "play", "pause", "stop", "fade", "slide",
            "scale", "rotate", "translate", "skew"};
}

std::vector<std::string> CHTLJSTokenUtils::getVirtualObjectKeywords() {
    return {"virtual", "vnode", "component", "mount", "unmount", "render", "update",
            "state", "props", "computed", "watch", "reactive", "ref", "refs",
            "emit", "dispatch", "lifecycle", "hook", "created", "mounted",
            "updated", "destroyed"};
}

std::vector<std::string> CHTLJSTokenUtils::getEventTypes() {
    return {"click", "dblclick", "mousedown", "mouseup", "mouseover", "mouseout",
            "mousemove", "keydown", "keyup", "keypress", "focus", "blur", "change",
            "input", "submit", "reset", "load", "unload", "resize", "scroll",
            "drag", "drop", "dragover", "touch", "touchstart", "touchend"};
}

bool CHTLJSTokenUtils::isValidIdentifier(const std::string& identifier) {
    if (identifier.empty()) return false;
    
    // 第一个字符必须是字母、下划线或$
    char first = identifier[0];
    if (!std::isalpha(first) && first != '_' && first != '$') {
        return false;
    }
    
    // 后续字符可以是字母、数字、下划线或$
    for (size_t i = 1; i < identifier.length(); ++i) {
        char c = identifier[i];
        if (!std::isalnum(c) && c != '_' && c != '$') {
            return false;
        }
    }
    
    // 不能是关键字
    return !isKeyword(identifier);
}

bool CHTLJSTokenUtils::requiresRuntimeSupport(CHTLJSTokenType type) {
    return isCHTLJSSpecificToken(type);
}

std::string CHTLJSTokenUtils::getRuntimeFeature(CHTLJSTokenType type) {
    if (getSelectorKeywordSet().find(type) != getSelectorKeywordSet().end()) {
        return "SelectorEngine";
    }
    if (getListenerKeywordSet().find(type) != getListenerKeywordSet().end()) {
        return "EventSystem";
    }
    if (getAnimationKeywordSet().find(type) != getAnimationKeywordSet().end()) {
        return "AnimationEngine";
    }
    if (getVirtualObjectKeywordSet().find(type) != getVirtualObjectKeywordSet().end()) {
        return "VirtualObjectSystem";
    }
    return "";
}

} // namespace CHTLJS