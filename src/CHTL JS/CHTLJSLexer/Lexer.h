#pragma once

#include "../../CHTL/CHTLLexer/Token.h"
#include <string>
#include <vector>

namespace CHTL {

class CHTLJSLexer {
public:
    CHTLJSLexer();
    ~CHTLJSLexer();
    
    std::vector<Token> tokenize(const std::string& code);
    
    void enableDebugMode(bool enable) { m_debugMode = enable; }
    
private:
    bool m_debugMode;
    
    bool isEnhancedSelector(const std::string& text);
    bool isEventBinding(const std::string& text);
};

}