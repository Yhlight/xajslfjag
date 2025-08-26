#ifndef CHTLJS_LEXER_H
#define CHTLJS_LEXER_H

#include <string>
#include <vector>
#include <memory>
#include "../chtl/CHTLLexer.h"
#include "CHTLJSToken.h"

namespace chtl {
namespace js {

// CHTL JS词法分析器
class CHTLJSLexer : public CHTLLexer {
public:
    CHTLJSLexer();
    explicit CHTLJSLexer(const LexerConfig& config);
    ~CHTLJSLexer();
    
    // 重写Token扫描方法
    CHTLJSToken nextJSToken();
    CHTLJSToken peekJSToken(int offset = 0);
    std::vector<CHTLJSToken> tokenizeJS();
    
    // CHTL JS特有的扫描方法
    CHTLJSToken scanSelector();
    CHTLJSToken scanArrowOperator();
    CHTLJSToken scanDoubleLeftBrace();
    CHTLJSToken scanDoubleRightBrace();
    CHTLJSToken scanVirtualObject();
    CHTLJSToken scanCHTLJSFunction();
    CHTLJSToken scanPlaceholder();
    
    // JavaScript基础扫描
    CHTLJSToken scanJSKeyword();
    CHTLJSToken scanJSOperator();
    CHTLJSToken scanJSLiteral();
    CHTLJSToken scanTemplateString();
    CHTLJSToken scanRegex();
    
    // 事件和动画相关
    bool isEventName(const std::string& text) const;
    bool isAnimationKeyword(const std::string& text) const;
    CHTLJSTokenType getAnimationKeywordType(const std::string& text) const;
    
private:
    // 辅助方法
    bool isValidSelectorStart(char c) const;
    bool isValidSelectorChar(char c) const;
    std::pair<std::string, std::string> parseSelectorContent(const std::string& content) const;
    
    // 转换基础Token为CHTL JS Token
    CHTLJSToken convertToken(const Token& token);
    
    // JavaScript关键字检查
    bool isJSKeyword(const std::string& text) const;
    CHTLJSTokenType getJSKeywordType(const std::string& text) const;
    
    // 操作符检查
    bool isJSOperator(const std::string& text) const;
    CHTLJSTokenType getJSOperatorType(const std::string& text) const;
    
    // 缓存
    mutable std::vector<CHTLJSToken> jsTokenBuffer;
    mutable size_t jsBufferIndex;
    
    // 关键字映射表
    static const std::unordered_map<std::string, CHTLJSTokenType> jsKeywords;
    static const std::unordered_map<std::string, CHTLJSTokenType> animationKeywords;
    static const std::unordered_set<std::string> eventNames;
};

} // namespace js
} // namespace chtl

#endif // CHTLJS_LEXER_H