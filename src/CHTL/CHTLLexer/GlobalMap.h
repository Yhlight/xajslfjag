#pragma once

#include "../../Util/Common.h"
#include "Token.h"

namespace CHTL {

// HTML元素映射表
class HTMLElementMap {
public:
    static bool isHTMLElement(const String& name);
    static bool isSingleTag(const String& name);
    static bool isBlockElement(const String& name);
    static bool isInlineElement(const String& name);
    
private:
    static const std::unordered_set<String> htmlElements;
    static const std::unordered_set<String> singleTags;
    static const std::unordered_set<String> blockElements;
    static const std::unordered_set<String> inlineElements;
};

// 关键字映射表
class KeywordMap {
public:
    static TokenType getKeywordType(const String& keyword);
    static bool isKeyword(const String& word);
    static bool isCHTLKeyword(const String& word);
    static bool isCHTLJSKeyword(const String& word);
    
private:
    static const StringUnorderedMap keywordMap;
    static const std::unordered_set<String> chtlKeywords;
    static const std::unordered_set<String> chtljsKeywords;
};

// CSS属性映射表
class CSSPropertyMap {
public:
    static bool isCSSProperty(const String& property);
    static bool isShorthandProperty(const String& property);
    static StringVector getShorthandProperties(const String& shorthand);
    
private:
    static const std::unordered_set<String> cssProperties;
    static const StringUnorderedMap shorthandMap;
};

// JavaScript事件映射表
class JSEventMap {
public:
    static bool isJSEvent(const String& event);
    static bool isMouseEvent(const String& event);
    static bool isKeyboardEvent(const String& event);
    static bool isFormEvent(const String& event);
    static StringVector getAllEvents();
    
private:
    static const std::unordered_set<String> allEvents;
    static const std::unordered_set<String> mouseEvents;
    static const std::unordered_set<String> keyboardEvents;
    static const std::unordered_set<String> formEvents;
};

// 全局符号映射表
class SymbolMap {
public:
    static TokenType getSymbolType(char c);
    static TokenType getTwoCharSymbolType(const String& symbol);
    static bool isOperatorChar(char c);
    static bool isSelectorChar(char c);
    
private:
    static const std::unordered_map<char, TokenType> singleCharMap;
    static const StringUnorderedMap twoCharMap;
};

// CJMOD API 语法映射
class CJMODSyntaxMap {
public:
    static bool isCJMODFunction(const String& name);
    static StringVector getCJMODParameters(const String& functionName);
    static bool isPlaceholder(const String& token);
    static String getPlaceholderType(const String& placeholder);
    
private:
    static const std::unordered_set<String> cjmodFunctions;
    static const StringUnorderedMap functionParameters;
    static const std::unordered_set<String> placeholders;
};

} // namespace CHTL