#pragma once

#include <memory>
#include <string>
#include <vector>
#include <istream>
#include <sstream>
#include "Token.h"
#include "GlobalMap.h"

namespace CHTL {

/// CHTL词法分析器类
class Lexer {
private:
    /// 输入源
    std::string source;
    
    /// 当前位置
    size_t position;
    
    /// 当前行号
    size_t currentLine;
    
    /// 当前列号
    size_t currentColumn;
    
    /// 全局映射表
    std::shared_ptr<GlobalMap> globalMap;
    
    /// Token缓存
    std::vector<Token> tokenBuffer;
    
    /// 是否已经完成词法分析
    bool isAnalyzed;

public:
    /// 构造函数
    explicit Lexer(std::shared_ptr<GlobalMap> gMap = nullptr);
    
    /// 析构函数
    ~Lexer();
    
    /// 设置源代码
    void SetSource(const std::string& sourceCode);
    
    /// 从流中设置源代码
    void SetSource(std::istream& stream);
    
    /// 执行词法分析
    void Analyze();
    
    /// 获取所有tokens
    const std::vector<Token>& GetTokens() const;
    
    /// 获取指定位置的token
    const Token& GetToken(size_t index) const;
    
    /// 获取token数量
    size_t GetTokenCount() const;
    
    /// 重置词法分析器
    void Reset();
    
    /// 检查是否已分析完成
    bool IsAnalyzed() const;
    
    /// 获取错误信息
    std::vector<std::string> GetErrors() const;
    
    /// 获取全局映射表
    std::shared_ptr<GlobalMap> GetGlobalMap() const { return globalMap; }

private:
    /// 跳过空白字符
    void SkipWhitespace();
    
    /// 读取下一个字符
    char PeekChar() const;
    
    /// 获取当前字符并移动位置
    char GetChar();
    
    /// 回退一个字符
    void UngetChar();
    
    /// 检查是否到达文件末尾
    bool IsEOF() const;
    
    /// 读取标识符或关键字
    Token ReadIdentifierOrKeyword();
    
    /// 读取数字
    Token ReadNumber();
    
    /// 读取字符串字面量
    Token ReadStringLiteral(char quote);
    
    /// 读取无修饰字面量
    Token ReadUnquotedLiteral();
    
    /// 读取单行注释
    Token ReadLineComment();
    
    /// 读取块注释
    Token ReadBlockComment();
    
    /// 读取生成器注释
    Token ReadGeneratorComment();
    
    /// 读取符号
    Token ReadSymbol();
    
    /// 读取括号类型（如[Template]）
    Token ReadBracketType();
    
    /// 读取类型标识符（如@Style）
    Token ReadTypeIdentifier();
    
    /// 检查是否为字母或下划线
    bool IsAlpha(char c) const;
    
    /// 检查是否为数字
    bool IsDigit(char c) const;
    
    /// 检查是否为字母数字或下划线
    bool IsAlphaNumeric(char c) const;
    
    /// 检查是否为空白字符
    bool IsWhitespace(char c) const;
    
    /// 检查是否为换行符
    bool IsNewline(char c) const;
    
    /// 检查是否为有效的无修饰字面量字符
    bool IsValidUnquotedChar(char c) const;
    
    /// 更新位置信息
    void UpdatePosition(char c);
    
    /// 创建Token
    Token CreateToken(TokenType type, const std::string& value = "");
    
    /// 添加Token到缓存
    void AddToken(const Token& token);
    
    /// 处理"at top"和"at bottom"这样的复合关键字
    void HandleCompoundKeywords();
    
    /// 验证Token序列的合法性
    bool ValidateTokenSequence() const;
    
    /// 错误信息收集
    std::vector<std::string> errors;
    
    /// 添加错误信息
    void AddError(const std::string& message);
};

} // namespace CHTL