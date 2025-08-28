/**
 * CHTL词法分析器
 * 
 * 负责将CHTL源代码转换为Token流
 * 严格按照CHTL语法文档进行词法分析
 * 
 * @author CHTL Team
 * @version 1.0.0
 * @license MIT
 */

#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include "Token.h"
#include "GlobalMap.h"

namespace CHTL {

/**
 * CHTL词法分析器类
 * 将源代码转换为Token流
 */
class CHTLLexer {
public:
    // 构造函数和析构函数
    CHTLLexer();
    explicit CHTLLexer(const std::string& source);
    explicit CHTLLexer(std::istream& input);
    ~CHTLLexer() = default;
    
    // 禁用拷贝和赋值
    CHTLLexer(const CHTLLexer&) = delete;
    CHTLLexer& operator=(const CHTLLexer&) = delete;
    
    // 设置输入源
    void setSource(const std::string& source);
    void setInput(std::istream& input);
    
    // 词法分析主函数
    std::vector<std::shared_ptr<Token>> tokenize();
    
    // 获取下一个Token
    std::shared_ptr<Token> getNextToken();
    
    // 查看下一个Token（不消耗）
    std::shared_ptr<Token> peekNextToken();
    
    // 重置词法分析器状态
    void reset();
    
    // 获取当前分析位置
    size_t getCurrentLine() const { return m_currentLine; }
    size_t getCurrentColumn() const { return m_currentColumn; }
    size_t getCurrentPosition() const { return m_currentPosition; }
    
    // 获取Token流
    const std::vector<std::shared_ptr<Token>>& getTokens() const { return m_tokens; }
    
    // 错误处理
    bool hasErrors() const { return !m_errors.empty(); }
    const std::vector<std::string>& getErrors() const { return m_errors; }
    void clearErrors() { m_errors.clear(); }
    
    // 调试信息
    void printTokens() const;
    std::string getTokenInfo() const;

private:
    // 输入源管理
    std::string m_source;                    // 源代码字符串
    std::istream* m_inputStream;             // 输入流指针
    bool m_useStringSource;                  // 是否使用字符串源
    
    // 分析状态
    size_t m_currentPosition;                // 当前字符位置
    size_t m_currentLine;                    // 当前行号
    size_t m_currentColumn;                  // 当前列号
    size_t m_startPosition;                  // 当前Token开始位置
    size_t m_startLine;                      // 当前Token开始行号
    size_t m_startColumn;                    // 当前Token开始列号
    
    // Token存储
    std::vector<std::shared_ptr<Token>> m_tokens;  // Token流
    size_t m_currentTokenIndex;              // 当前Token索引
    
    // 错误信息
    std::vector<std::string> m_errors;      // 错误信息列表
    
    // 全局映射表引用
    GlobalMap& m_globalMap;                 // 全局映射表引用
    
    // 私有方法
    void initialize();
    
    // 字符读取
    char getCurrentChar() const;
    char getNextChar();
    char peekNextChar() const;
    char peekChar(size_t offset) const;
    bool isAtEnd() const;
    void advance();
    
    // Token识别方法
    std::shared_ptr<Token> recognizeToken();
    std::shared_ptr<Token> recognizeComment();
    std::shared_ptr<Token> recognizeStringLiteral();
    std::shared_ptr<Token> recognizeUnquotedLiteral();
    std::shared_ptr<Token> recognizeNumberLiteral();
    std::shared_ptr<Token> recognizeIdentifier();
    std::shared_ptr<Token> recognizeOperator();
    std::shared_ptr<Token> recognizeDelimiter();
    std::shared_ptr<Token> recognizeSpecialSymbol();
    std::shared_ptr<Token> recognizeKeyword();
    std::shared_ptr<Token> recognizeWhitespace();
    std::shared_ptr<Token> recognizeNewline();
    
    // 辅助方法
    bool isWhitespace(char ch) const;
    bool isNewline(char ch) const;
    bool isDigit(char ch) const;
    bool isLetter(char ch) const;
    bool isIdentifierStart(char ch) const;
    bool isIdentifierPart(char ch) const;
    bool isHexDigit(char ch) const;
    
    // 注释处理
    std::shared_ptr<Token> processSingleLineComment();
    std::shared_ptr<Token> processMultiLineComment();
    std::shared_ptr<Token> processGeneratorComment();
    
    // 字符串处理
    std::shared_ptr<Token> processDoubleQuotedString();
    std::shared_ptr<Token> processSingleQuotedString();
    
    // 数字处理
    std::shared_ptr<Token> processInteger();
    std::shared_ptr<Token> processFloat();
    std::shared_ptr<Token> processHexNumber();
    
    // 标识符处理
    std::shared_ptr<Token> processIdentifier();
    
    // 关键字识别
    std::shared_ptr<Token> processKeyword();
    
    // 错误处理
    void addError(const std::string& error);
    void addError(const std::string& error, size_t line, size_t column);
    
    // 位置管理
    void updatePosition();
    void saveTokenStart();
    void restoreTokenStart();
    
    // 工具方法
    std::string extractString(size_t start, size_t end) const;
    void skipWhitespace();
    bool matchString(const std::string& str);
    bool matchKeyword(const std::string& keyword);
};

} // namespace CHTL

#endif // CHTL_LEXER_H