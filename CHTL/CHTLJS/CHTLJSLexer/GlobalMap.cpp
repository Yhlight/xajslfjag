#include "GlobalMap.h"

namespace CHTLJS {

GlobalMap* GlobalMap::instance = nullptr;

GlobalMap::GlobalMap() {
    initializeKeywords();
    initializeCHTLJSFunctions();
    initializeEasingFunctions();
}

GlobalMap* GlobalMap::getInstance() {
    if (instance == nullptr) {
        instance = new GlobalMap();
    }
    return instance;
}

void GlobalMap::destroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

TokenType GlobalMap::getKeywordType(const std::string& word) const {
    auto it = keywordMap.find(word);
    if (it != keywordMap.end()) {
        return it->second;
    }
    return TokenType::UNKNOWN;
}

bool GlobalMap::isCHTLJSFunction(const std::string& func) const {
    return chtljsFunctions.find(func) != chtljsFunctions.end();
}

bool GlobalMap::isEasingFunction(const std::string& func) const {
    return easingFunctions.find(func) != easingFunctions.end();
}

void GlobalMap::initializeKeywords() {
    // JavaScript 基础关键字
    keywordMap["const"] = TokenType::KEYWORD_CONST;
    keywordMap["let"] = TokenType::KEYWORD_LET;
    keywordMap["var"] = TokenType::KEYWORD_VAR;
    keywordMap["function"] = TokenType::KEYWORD_FUNCTION;
    keywordMap["return"] = TokenType::KEYWORD_RETURN;
    keywordMap["if"] = TokenType::KEYWORD_IF;
    keywordMap["else"] = TokenType::KEYWORD_ELSE;
    keywordMap["for"] = TokenType::KEYWORD_FOR;
    keywordMap["while"] = TokenType::KEYWORD_WHILE;
    keywordMap["do"] = TokenType::KEYWORD_DO;
    keywordMap["break"] = TokenType::KEYWORD_BREAK;
    keywordMap["continue"] = TokenType::KEYWORD_CONTINUE;
    keywordMap["true"] = TokenType::KEYWORD_TRUE;
    keywordMap["false"] = TokenType::KEYWORD_FALSE;
    keywordMap["null"] = TokenType::KEYWORD_NULL;
    
    // CHTL JS 特殊关键字
    keywordMap["vir"] = TokenType::KEYWORD_VIR;
    keywordMap["listen"] = TokenType::KEYWORD_LISTEN;
    keywordMap["animate"] = TokenType::KEYWORD_ANIMATE;
    keywordMap["delegate"] = TokenType::KEYWORD_DELEGATE;
    keywordMap["target"] = TokenType::KEYWORD_TARGET;
    keywordMap["duration"] = TokenType::KEYWORD_DURATION;
    keywordMap["easing"] = TokenType::KEYWORD_EASING;
    keywordMap["begin"] = TokenType::KEYWORD_BEGIN;
    keywordMap["when"] = TokenType::KEYWORD_WHEN;
    keywordMap["end"] = TokenType::KEYWORD_END;
    keywordMap["at"] = TokenType::KEYWORD_AT;
    keywordMap["loop"] = TokenType::KEYWORD_LOOP;
    keywordMap["direction"] = TokenType::KEYWORD_DIRECTION;
    keywordMap["delay"] = TokenType::KEYWORD_DELAY;
    keywordMap["callback"] = TokenType::KEYWORD_CALLBACK;
}

void GlobalMap::initializeCHTLJSFunctions() {
    // CHTL JS 核心函数
    chtljsFunctions.insert("listen");
    chtljsFunctions.insert("animate");
    chtljsFunctions.insert("delegate");
    
    // 注意：printMylove和iNeverAway不是内置函数
    // 它们是Chtholly模块通过CJMOD提供的扩展功能
}

void GlobalMap::initializeEasingFunctions() {
    // 标准缓动函数
    easingFunctions.insert("linear");
    easingFunctions.insert("ease");
    easingFunctions.insert("ease-in");
    easingFunctions.insert("ease-out");
    easingFunctions.insert("ease-in-out");
    
    // 贝塞尔曲线缓动
    easingFunctions.insert("cubic-bezier");
    
    // 扩展缓动函数
    easingFunctions.insert("ease-in-quad");
    easingFunctions.insert("ease-out-quad");
    easingFunctions.insert("ease-in-out-quad");
    easingFunctions.insert("ease-in-cubic");
    easingFunctions.insert("ease-out-cubic");
    easingFunctions.insert("ease-in-out-cubic");
    easingFunctions.insert("ease-in-quart");
    easingFunctions.insert("ease-out-quart");
    easingFunctions.insert("ease-in-out-quart");
    easingFunctions.insert("ease-in-quint");
    easingFunctions.insert("ease-out-quint");
    easingFunctions.insert("ease-in-out-quint");
    easingFunctions.insert("ease-in-sine");
    easingFunctions.insert("ease-out-sine");
    easingFunctions.insert("ease-in-out-sine");
    easingFunctions.insert("ease-in-expo");
    easingFunctions.insert("ease-out-expo");
    easingFunctions.insert("ease-in-out-expo");
    easingFunctions.insert("ease-in-circ");
    easingFunctions.insert("ease-out-circ");
    easingFunctions.insert("ease-in-out-circ");
    easingFunctions.insert("ease-in-elastic");
    easingFunctions.insert("ease-out-elastic");
    easingFunctions.insert("ease-in-out-elastic");
    easingFunctions.insert("ease-in-back");
    easingFunctions.insert("ease-out-back");
    easingFunctions.insert("ease-in-out-back");
    easingFunctions.insert("ease-in-bounce");
    easingFunctions.insert("ease-out-bounce");
    easingFunctions.insert("ease-in-out-bounce");
}

void GlobalMap::reset() {
    keywordMap.clear();
    chtljsFunctions.clear();
    easingFunctions.clear();
    initializeKeywords();
    initializeCHTLJSFunctions();
    initializeEasingFunctions();
}

} // namespace CHTLJS