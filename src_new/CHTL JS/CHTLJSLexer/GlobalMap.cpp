#include "GlobalMap.h"

namespace CHTLJS {

GlobalMap& GlobalMap::getInstance() {
    static GlobalMap instance;
    if (!instance.m_initialized) {
        instance.initializeCHTLJSData();
        instance.m_initialized = true;
    }
    return instance;
}

GlobalMap::GlobalMap() : m_initialized(false) {
}

void GlobalMap::initializeCHTLJSData() {
    // 初始化CHTL JS关键字
    m_chtlJSKeywords = {
        "script", "module", "load", "listen", "delegate", 
        "animate", "vir"
    };
    
    // 初始化JavaScript事件
    m_jsEvents = {
        "click", "mouseenter", "mouseleave", "keydown", "keyup",
        "submit", "change", "focus", "blur", "resize", "scroll",
        "touchstart", "touchend", "touchmove"
    };
    
    // 初始化增强选择器模式
    m_enhancedSelectorPatterns = {
        "{{.", "{{#", "{{[", "{{"
    };
}

bool GlobalMap::isCHTLJSKeyword(const std::string& word) const {
    return m_chtlJSKeywords.find(word) != m_chtlJSKeywords.end();
}

bool GlobalMap::isJSEvent(const std::string& event) const {
    return m_jsEvents.find(event) != m_jsEvents.end();
}

bool GlobalMap::isEnhancedSelector(const std::string& selector) const {
    for (const auto& pattern : m_enhancedSelectorPatterns) {
        if (selector.find(pattern) == 0) {
            return true;
        }
    }
    return false;
}

bool GlobalMap::isCHTLJSFunction(const std::string& syntax) const {
    // 检查CHTL JS函数语法: functionName {key: value}
    // 简化检查：包含标识符和大括号
    size_t bracePos = syntax.find('{');
    if (bracePos == std::string::npos) return false;
    
    // 检查前面是否有标识符
    std::string beforeBrace = syntax.substr(0, bracePos);
    // 移除空白字符
    size_t start = beforeBrace.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return false;
    size_t end = beforeBrace.find_last_not_of(" \t\n\r");
    beforeBrace = beforeBrace.substr(start, end - start + 1);
    
    // 简单检查是否为标识符
    if (beforeBrace.empty()) return false;
    char first = beforeBrace[0];
    return (first >= 'a' && first <= 'z') || (first >= 'A' && first <= 'Z') || first == '_';
}

} // namespace CHTLJS