#include "CHTLJS/Token.h"
#include <algorithm>

namespace CHTLJS {

// Token实现
bool Token::IsCHTLJSKeyword() const {
    return type >= TokenType::LISTEN && type <= TokenType::FILL;
}

bool Token::IsJavaScriptKeyword() const {
    return type >= TokenType::VAR && type <= TokenType::THROW;
}

bool Token::IsEventName() const {
    return type >= TokenType::EVENT_CLICK && type <= TokenType::EVENT_CUSTOM;
}

bool Token::IsLiteral() const {
    return type == TokenType::IDENTIFIER ||
           type == TokenType::STRING_LITERAL ||
           type == TokenType::SINGLE_STRING_LITERAL ||
           type == TokenType::UNQUOTED_LITERAL ||
           type == TokenType::NUMBER ||
           type == TokenType::BOOLEAN ||
           type == TokenType::NULL_LITERAL ||
           type == TokenType::UNDEFINED;
}

bool Token::IsOperator() const {
    return type == TokenType::PLUS ||
           type == TokenType::MINUS ||
           type == TokenType::MULTIPLY ||
           type == TokenType::DIVIDE ||
           type == TokenType::MODULO ||
           type == TokenType::EQUALS ||
           type == TokenType::EXCLAMATION ||
           type == TokenType::QUESTION ||
           type == TokenType::ARROW ||
           type == TokenType::DOUBLE_COLON;
}

bool Token::IsComment() const {
    return type == TokenType::LINE_COMMENT ||
           type == TokenType::BLOCK_COMMENT;
}

String Token::GetTypeName() const {
    return TokenUtils::TokenTypeToString(type);
}

// TokenUtils实现
TokenType TokenUtils::StringToTokenType(const String& str) {
    // CHTL JS关键字映射
    static const HashMap<String, TokenType> chtljsKeywordMap = {
        {"listen", TokenType::LISTEN},
        {"delegate", TokenType::DELEGATE},
        {"animate", TokenType::ANIMATE},
        {"vir", TokenType::VIR},
        {"from", TokenType::FROM},
        {"to", TokenType::TO},
        {"by", TokenType::BY},
        {"duration", TokenType::DURATION},
        {"delay", TokenType::DELAY},
        {"easing", TokenType::EASING},
        {"repeat", TokenType::REPEAT},
        {"alternate", TokenType::ALTERNATE},
        {"fill", TokenType::FILL}
    };
    
    // JavaScript关键字映射
    static const HashMap<String, TokenType> jsKeywordMap = {
        {"var", TokenType::VAR},
        {"let", TokenType::LET},
        {"const", TokenType::CONST},
        {"function", TokenType::FUNCTION},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"for", TokenType::FOR},
        {"while", TokenType::WHILE},
        {"do", TokenType::DO},
        {"return", TokenType::RETURN},
        {"this", TokenType::THIS},
        {"new", TokenType::NEW},
        {"try", TokenType::TRY},
        {"catch", TokenType::CATCH},
        {"finally", TokenType::FINALLY},
        {"throw", TokenType::THROW},
        {"true", TokenType::BOOLEAN},
        {"false", TokenType::BOOLEAN},
        {"null", TokenType::NULL_LITERAL},
        {"undefined", TokenType::UNDEFINED}
    };
    
    // 事件名映射
    static const HashMap<String, TokenType> eventMap = {
        {"click", TokenType::EVENT_CLICK},
        {"dblclick", TokenType::EVENT_DBLCLICK},
        {"mousedown", TokenType::EVENT_MOUSEDOWN},
        {"mouseup", TokenType::EVENT_MOUSEUP},
        {"mousemove", TokenType::EVENT_MOUSEMOVE},
        {"mouseover", TokenType::EVENT_MOUSEOVER},
        {"mouseout", TokenType::EVENT_MOUSEOUT},
        {"mouseenter", TokenType::EVENT_MOUSEENTER},
        {"mouseleave", TokenType::EVENT_MOUSELEAVE},
        {"keydown", TokenType::EVENT_KEYDOWN},
        {"keyup", TokenType::EVENT_KEYUP},
        {"keypress", TokenType::EVENT_KEYPRESS},
        {"focus", TokenType::EVENT_FOCUS},
        {"blur", TokenType::EVENT_BLUR},
        {"change", TokenType::EVENT_CHANGE},
        {"input", TokenType::EVENT_INPUT},
        {"submit", TokenType::EVENT_SUBMIT},
        {"reset", TokenType::EVENT_RESET},
        {"load", TokenType::EVENT_LOAD},
        {"unload", TokenType::EVENT_UNLOAD},
        {"resize", TokenType::EVENT_RESIZE},
        {"scroll", TokenType::EVENT_SCROLL}
    };
    
    // 检查CHTL JS关键字
    auto chtljsIt = chtljsKeywordMap.find(str);
    if (chtljsIt != chtljsKeywordMap.end()) {
        return chtljsIt->second;
    }
    
    // 检查JavaScript关键字
    auto jsIt = jsKeywordMap.find(str);
    if (jsIt != jsKeywordMap.end()) {
        return jsIt->second;
    }
    
    // 检查事件名
    auto eventIt = eventMap.find(str);
    if (eventIt != eventMap.end()) {
        return eventIt->second;
    }
    
    // 特殊处理
    if (str == "{{") return TokenType::DOUBLE_LEFT_BRACE;
    if (str == "}}") return TokenType::DOUBLE_RIGHT_BRACE;
    if (str == "->") return TokenType::ARROW;
    if (str == "::") return TokenType::DOUBLE_COLON;
    
    return TokenType::UNKNOWN;
}

String TokenUtils::TokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::EOF_TOKEN: return "EOF";
        case TokenType::UNKNOWN: return "UNKNOWN";
        
        // 字面量
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::SINGLE_STRING_LITERAL: return "SINGLE_STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::BOOLEAN: return "BOOLEAN";
        case TokenType::NULL_LITERAL: return "NULL";
        case TokenType::UNDEFINED: return "UNDEFINED";
        
        // 运算符和标点
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::EXCLAMATION: return "EXCLAMATION";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::MODULO: return "MODULO";
        
        // CHTL JS特有标记
        case TokenType::DOUBLE_LEFT_BRACE: return "DOUBLE_LEFT_BRACE";
        case TokenType::DOUBLE_RIGHT_BRACE: return "DOUBLE_RIGHT_BRACE";
        case TokenType::ARROW: return "ARROW";
        case TokenType::DOUBLE_COLON: return "DOUBLE_COLON";
        
        // CHTL JS关键字
        case TokenType::LISTEN: return "LISTEN";
        case TokenType::DELEGATE: return "DELEGATE";
        case TokenType::ANIMATE: return "ANIMATE";
        case TokenType::VIR: return "VIR";
        case TokenType::FROM: return "FROM";
        case TokenType::TO: return "TO";
        case TokenType::BY: return "BY";
        case TokenType::DURATION: return "DURATION";
        case TokenType::DELAY: return "DELAY";
        case TokenType::EASING: return "EASING";
        case TokenType::REPEAT: return "REPEAT";
        case TokenType::ALTERNATE: return "ALTERNATE";
        case TokenType::FILL: return "FILL";
        
        // JavaScript关键字
        case TokenType::VAR: return "VAR";
        case TokenType::LET: return "LET";
        case TokenType::CONST: return "CONST";
        case TokenType::FUNCTION: return "FUNCTION";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::FOR: return "FOR";
        case TokenType::WHILE: return "WHILE";
        case TokenType::DO: return "DO";
        case TokenType::RETURN: return "RETURN";
        case TokenType::THIS: return "THIS";
        case TokenType::NEW: return "NEW";
        case TokenType::TRY: return "TRY";
        case TokenType::CATCH: return "CATCH";
        case TokenType::FINALLY: return "FINALLY";
        case TokenType::THROW: return "THROW";
        
        // 事件名
        case TokenType::EVENT_CLICK: return "EVENT_CLICK";
        case TokenType::EVENT_DBLCLICK: return "EVENT_DBLCLICK";
        case TokenType::EVENT_MOUSEDOWN: return "EVENT_MOUSEDOWN";
        case TokenType::EVENT_MOUSEUP: return "EVENT_MOUSEUP";
        case TokenType::EVENT_MOUSEMOVE: return "EVENT_MOUSEMOVE";
        case TokenType::EVENT_MOUSEOVER: return "EVENT_MOUSEOVER";
        case TokenType::EVENT_MOUSEOUT: return "EVENT_MOUSEOUT";
        case TokenType::EVENT_MOUSEENTER: return "EVENT_MOUSEENTER";
        case TokenType::EVENT_MOUSELEAVE: return "EVENT_MOUSELEAVE";
        case TokenType::EVENT_KEYDOWN: return "EVENT_KEYDOWN";
        case TokenType::EVENT_KEYUP: return "EVENT_KEYUP";
        case TokenType::EVENT_KEYPRESS: return "EVENT_KEYPRESS";
        case TokenType::EVENT_FOCUS: return "EVENT_FOCUS";
        case TokenType::EVENT_BLUR: return "EVENT_BLUR";
        case TokenType::EVENT_CHANGE: return "EVENT_CHANGE";
        case TokenType::EVENT_INPUT: return "EVENT_INPUT";
        case TokenType::EVENT_SUBMIT: return "EVENT_SUBMIT";
        case TokenType::EVENT_RESET: return "EVENT_RESET";
        case TokenType::EVENT_LOAD: return "EVENT_LOAD";
        case TokenType::EVENT_UNLOAD: return "EVENT_UNLOAD";
        case TokenType::EVENT_RESIZE: return "EVENT_RESIZE";
        case TokenType::EVENT_SCROLL: return "EVENT_SCROLL";
        case TokenType::EVENT_CUSTOM: return "EVENT_CUSTOM";
        
        // 注释
        case TokenType::LINE_COMMENT: return "LINE_COMMENT";
        case TokenType::BLOCK_COMMENT: return "BLOCK_COMMENT";
        
        // 特殊
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::NEWLINE: return "NEWLINE";
        
        default: return "UNKNOWN";
    }
}

bool TokenUtils::IsCHTLJSKeyword(const String& str) {
    static const HashSet<String> keywords = {
        "listen", "delegate", "animate", "vir",
        "from", "to", "by", "duration", "delay",
        "easing", "repeat", "alternate", "fill"
    };
    return keywords.find(str) != keywords.end();
}

bool TokenUtils::IsJavaScriptKeyword(const String& str) {
    static const HashSet<String> keywords = {
        "var", "let", "const", "function", "if", "else",
        "for", "while", "do", "return", "this", "new",
        "try", "catch", "finally", "throw",
        "true", "false", "null", "undefined"
    };
    return keywords.find(str) != keywords.end();
}

bool TokenUtils::IsEventName(const String& str) {
    static const HashSet<String> events = {
        "click", "dblclick", "mousedown", "mouseup", "mousemove",
        "mouseover", "mouseout", "mouseenter", "mouseleave",
        "keydown", "keyup", "keypress", "focus", "blur",
        "change", "input", "submit", "reset", "load",
        "unload", "resize", "scroll",
        // 支持更多事件
        "touchstart", "touchmove", "touchend", "touchcancel",
        "wheel", "contextmenu", "drag", "dragstart", "dragend",
        "dragover", "dragenter", "dragleave", "drop",
        "animationstart", "animationend", "animationiteration",
        "transitionstart", "transitionend", "transitionrun",
        "pointerdown", "pointerup", "pointermove", "pointerenter",
        "pointerleave", "pointerover", "pointerout"
    };
    return events.find(str) != events.end();
}

Vector<String> TokenUtils::GetCHTLJSKeywords() {
    return {
        "listen", "delegate", "animate", "vir",
        "from", "to", "by", "duration", "delay",
        "easing", "repeat", "alternate", "fill"
    };
}

Vector<String> TokenUtils::GetJavaScriptKeywords() {
    return {
        "var", "let", "const", "function", "if", "else",
        "for", "while", "do", "return", "this", "new",
        "try", "catch", "finally", "throw",
        "true", "false", "null", "undefined"
    };
}

Vector<String> TokenUtils::GetEventNames() {
    return {
        // 鼠标事件
        "click", "dblclick", "mousedown", "mouseup", "mousemove",
        "mouseover", "mouseout", "mouseenter", "mouseleave",
        // 键盘事件
        "keydown", "keyup", "keypress",
        // 焦点事件
        "focus", "blur", "focusin", "focusout",
        // 表单事件
        "change", "input", "submit", "reset", "select",
        // 窗口事件
        "load", "unload", "resize", "scroll",
        // 触摸事件
        "touchstart", "touchmove", "touchend", "touchcancel",
        // 拖拽事件
        "drag", "dragstart", "dragend", "dragover",
        "dragenter", "dragleave", "drop",
        // 动画事件
        "animationstart", "animationend", "animationiteration",
        "transitionstart", "transitionend", "transitionrun",
        // 指针事件
        "pointerdown", "pointerup", "pointermove",
        "pointerenter", "pointerleave", "pointerover", "pointerout",
        // 其他事件
        "wheel", "contextmenu", "error", "abort"
    };
}

String TokenUtils::NormalizeEventName(const String& eventName) {
    String normalized = eventName;
    
    // 移除 "on" 前缀
    if (normalized.length() > 2 && normalized.substr(0, 2) == "on") {
        normalized = normalized.substr(2);
    }
    
    // 转换为小写
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    
    return normalized;
}

// TokenStream实现
void TokenStream::AddToken(const Token& token) {
    tokens_.push_back(token);
}

void TokenStream::AddToken(TokenType type, const String& value, const SourceLocation& location) {
    tokens_.emplace_back(type, value, location);
}

const Token& TokenStream::Current() const {
    if (IsAtEnd()) {
        static Token eofToken(TokenType::EOF_TOKEN, "", SourceLocation());
        return eofToken;
    }
    return tokens_[current_];
}

const Token& TokenStream::Next() {
    if (!IsAtEnd()) {
        current_++;
    }
    return Current();
}

const Token& TokenStream::Peek(size_t offset) const {
    size_t pos = current_ + offset;
    if (pos >= tokens_.size()) {
        static Token eofToken(TokenType::EOF_TOKEN, "", SourceLocation());
        return eofToken;
    }
    return tokens_[pos];
}

bool TokenStream::IsAtEnd() const {
    return current_ >= tokens_.size();
}

void TokenStream::Clear() {
    tokens_.clear();
    current_ = 0;
}

bool TokenStream::Match(TokenType type) {
    if (IsAtEnd()) return false;
    if (Current().type == type) {
        Next();
        return true;
    }
    return false;
}

bool TokenStream::Match(const Vector<TokenType>& types) {
    for (TokenType type : types) {
        if (Match(type)) {
            return true;
        }
    }
    return false;
}

Token TokenStream::Consume(TokenType type, const String& errorMessage) {
    if (IsAtEnd() || Current().type != type) {
        throw SyntaxError(errorMessage, Current().location);
    }
    Token token = Current();
    Next();
    return token;
}

void TokenStream::Synchronize() {
    // 错误恢复：跳到下一个语句开始位置
    while (!IsAtEnd()) {
        if (Current().type == TokenType::SEMICOLON) {
            Next();
            return;
        }
        
        switch (Peek().type) {
            case TokenType::LISTEN:
            case TokenType::DELEGATE:
            case TokenType::ANIMATE:
            case TokenType::VIR:
            case TokenType::VAR:
            case TokenType::LET:
            case TokenType::CONST:
            case TokenType::FUNCTION:
            case TokenType::IF:
            case TokenType::FOR:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        
        Next();
    }
}

bool TokenStream::MatchEnhancedSelector() {
    return Match(TokenType::DOUBLE_LEFT_BRACE);
}

bool TokenStream::MatchArrowOperator() {
    return Match(TokenType::ARROW);
}

bool TokenStream::MatchEventName() {
    if (IsAtEnd()) return false;
    return Current().IsEventName();
}

} // namespace CHTLJS