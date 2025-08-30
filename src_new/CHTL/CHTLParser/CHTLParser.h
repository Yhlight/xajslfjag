#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../CHTLNode/BaseNode.h"
#include "../CHTLContext/Context.h"
#include "../CHTLLexer/Token.h"

namespace CHTL {

class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser();
    
    // 主解析接口
    std::shared_ptr<BaseNode> parse(const std::string& code);
    std::shared_ptr<BaseNode> parse(const std::vector<Token>& tokens);
    
    // 解析控制
    void setContext(std::shared_ptr<CHTLContext> context);
    std::shared_ptr<CHTLContext> getContext() const;
    
    // 解析选项
    void enableDebugMode(bool enable) { m_debugMode = enable; }
    void enableStrictMode(bool enable) { m_strictMode = enable; }
    
    // 错误处理
    std::vector<std::string> getErrors() const { return m_errors; }
    std::vector<std::string> getWarnings() const { return m_warnings; }
    void clearErrors();
    
private:
    std::shared_ptr<CHTLContext> m_context;
    std::vector<Token> m_tokens;
    size_t m_currentTokenIndex;
    bool m_debugMode;
    bool m_strictMode;
    std::vector<std::string> m_errors;
    std::vector<std::string> m_warnings;
    
    // 解析方法
    std::shared_ptr<BaseNode> parseDocument();
    std::shared_ptr<BaseNode> parseElement();
    std::shared_ptr<BaseNode> parseTemplate();
    std::shared_ptr<BaseNode> parseCustom();
    std::shared_ptr<BaseNode> parseOrigin();
    std::shared_ptr<BaseNode> parseImport();
    std::shared_ptr<BaseNode> parseNamespace();
    std::shared_ptr<BaseNode> parseConfiguration();
    std::shared_ptr<BaseNode> parseStyleBlock();
    std::shared_ptr<BaseNode> parseScriptBlock();
    std::shared_ptr<BaseNode> parseTextNode();
    
    // Token操作
    Token getCurrentToken() const;
    Token getNextToken();
    Token peekToken(size_t offset = 1) const;
    bool hasMoreTokens() const;
    void consumeToken();
    bool expectToken(TokenType type);
    bool matchToken(TokenType type) const;
    
    // 辅助方法
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
    bool isKeyword(const std::string& word) const;
    std::string getTokenValue(const Token& token) const;
};

}