#pragma once

#include "CHTLToken.h"
#include "CHTLGlobalMap.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// CHTL词法分析器
class CHTLLexer {
public:
    CHTLLexer();
    ~CHTLLexer() = default;
    
    // 设置源代码
    void setSource(const std::string& source);
    
    // 获取下一个Token
    std::shared_ptr<CHTLToken> getNextToken();
    
    // 获取所有Token
    std::vector<std::shared_ptr<CHTLToken>> getAllTokens();
    
    // 重置词法分析器
    void reset();
    
    // 获取当前行号和列号
    size_t getCurrentLine() const { return currentLine_; }
    size_t getCurrentColumn() const { return currentColumn_; }
    
    // 获取源代码
    const std::string& getSource() const { return source_; }
    
    // 检查是否到达文件末尾
    bool isEOF() const { return currentPos_ >= source_.length(); }
    
private:
    std::string source_;
    size_t currentPos_;
    size_t currentLine_;
    size_t currentColumn_;
    std::shared_ptr<CHTLGlobalMap> globalMap_;
    
    // 辅助函数
    void skipWhitespace();
    void skipComment();
    void skipSingleLineComment();
    void skipMultiLineComment();
    void skipGeneratorComment();
    
    std::shared_ptr<CHTLToken> readIdentifier();
    std::shared_ptr<CHTLToken> readString();
    std::shared_ptr<CHTLToken> readUnquotedLiteral();
    std::shared_ptr<CHTLToken> readNumber();
    
    CHTLTokenType getKeywordType(const std::string& identifier);
    CHTLTokenType getTemplateType(const std::string& identifier);
    CHTLTokenType getCustomType(const std::string& identifier);
    CHTLTokenType getOriginType(const std::string& identifier);
    CHTLTokenType getImportType(const std::string& identifier);
    
    bool isKeyword(const std::string& identifier);
    bool isTemplate(const std::string& identifier);
    bool isCustom(const std::string& identifier);
    bool isOrigin(const std::string& identifier);
    bool isImport(const std::string& identifier);
    
    void advance();
    void advanceLine();
    char peek(size_t offset = 0) const;
    bool match(const std::string& expected);
};

} // namespace CHTL