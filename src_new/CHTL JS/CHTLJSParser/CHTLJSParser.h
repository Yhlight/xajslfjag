#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../CHTLJSNode/BaseNode.h"
#include "../CHTLJSContext/Context.h"
#include "../../CHTL/CHTLLexer/Token.h"

namespace CHTL {

class CHTLJSParser {
public:
    CHTLJSParser();
    ~CHTLJSParser();
    
    // 主解析接口
    std::shared_ptr<BaseNode> parse(const std::string& code);
    std::shared_ptr<BaseNode> parse(const std::vector<Token>& tokens);
    
    // 上下文管理
    void setContext(std::shared_ptr<CHTLJSContext> context);
    std::shared_ptr<CHTLJSContext> getContext() const;
    
    // 调试
    void enableDebugMode(bool enable) { m_debugMode = enable; }
    std::vector<std::string> getErrors() const { return m_errors; }
    std::vector<std::string> getWarnings() const { return m_warnings; }
    void clearErrors();
    
private:
    std::shared_ptr<CHTLJSContext> m_context;
    std::vector<Token> m_tokens;
    size_t m_currentTokenIndex;
    bool m_debugMode;
    std::vector<std::string> m_errors;
    std::vector<std::string> m_warnings;
    
    // 解析方法
    std::shared_ptr<BaseNode> parseDocument();
    std::shared_ptr<BaseNode> parseModuleImport();
    std::shared_ptr<BaseNode> parseAnimateBlock();
    std::shared_ptr<BaseNode> parseListenBlock();
    std::shared_ptr<BaseNode> parseVirtualObject();
    std::shared_ptr<BaseNode> parseEnhancedSelector();
    std::shared_ptr<BaseNode> parseEventBinding();
    std::shared_ptr<BaseNode> parseDelegateBlock();
    std::shared_ptr<BaseNode> parseJavaScriptStatement();
    
    // 辅助方法
    std::string collectFunctionContent();
    std::string collectParameterValue();
    std::string collectArrayContent();
    bool isEnhancedSelector(const std::string& value);
    bool isEventBinding();
    
    // Token操作
    Token getCurrentToken() const;
    Token peekToken(size_t offset = 1) const;
    bool hasMoreTokens() const;
    void consumeToken();
    bool expectToken(TokenType type);
    bool matchToken(TokenType type) const;
    
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
};

}