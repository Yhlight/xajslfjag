#include "Lexer.h"

namespace CHTL {

CHTLJSLexer::CHTLJSLexer() : m_debugMode(false) {}
CHTLJSLexer::~CHTLJSLexer() = default;

std::vector<Token> CHTLJSLexer::tokenize(const std::string& code) {
    std::vector<Token> tokens;
    
    // 基本的CHTL JS词法分析
    // 这里可以复用CHTL的Lexer并添加CHTL JS特有的Token识别
    
    return tokens;
}

bool CHTLJSLexer::isEnhancedSelector(const std::string& text) {
    return text.find("{{") != std::string::npos && text.find("}}") != std::string::npos;
}

bool CHTLJSLexer::isEventBinding(const std::string& text) {
    return text.find("&->") != std::string::npos;
}

}