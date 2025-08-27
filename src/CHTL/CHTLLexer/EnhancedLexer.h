#pragma once

#include "Token.h"
#include "GlobalMap.h"
#include <memory>
#include <vector>
#include <string>
#include <stack>

namespace CHTL {

/// 词法分析上下文类型
enum class LexerContext {
    GLOBAL,          // 全局上下文
    ELEMENT_CONTENT, // 元素内容上下文
    STYLE_BLOCK,     // 样式块上下文
    SCRIPT_BLOCK,    // 脚本块上下文
    TEXT_BLOCK,      // 文本块上下文
    ATTRIBUTE_VALUE, // 属性值上下文
    SELECTOR,        // 选择器上下文
    CSS_PROPERTY,    // CSS属性上下文
    CSS_VALUE,       // CSS值上下文
    STRING_LITERAL   // 字符串字面量上下文
};

/// 增强型CHTL词法分析器
/// 支持上下文感知的Token识别
class EnhancedLexer {
private:
    std::string source;                          // 源代码
    size_t position;                             // 当前位置
    size_t currentLine;                          // 当前行号
    size_t currentColumn;                        // 当前列号
    
    std::shared_ptr<GlobalMap> globalMap;        // 全局映射表
    std::vector<Token> tokenBuffer;              // Token缓存
    std::vector<std::string> errors;             // 错误信息
    std::stack<LexerContext> contextStack;       // 上下文栈
    
    bool isAnalyzed;                             // 是否已分析
    
public:
    /// 构造函数
    explicit EnhancedLexer(std::shared_ptr<GlobalMap> gMap = nullptr);
    
    /// 析构函数
    ~EnhancedLexer();
    
    /// 设置源代码
    void SetSource(const std::string& sourceCode);
    
    /// 执行词法分析
    void Analyze();
    
    /// 获取所有tokens
    const std::vector<Token>& GetTokens() const;
    
    /// 获取错误信息
    const std::vector<std::string>& GetErrors() const;
    
    /// 检查是否有错误
    bool HasErrors() const { return !errors.empty(); }

private:
    // === 基础字符处理 ===
    
    /// 查看当前字符
    char PeekChar() const;
    
    /// 获取当前字符并前进
    char GetChar();
    
    /// 检查是否到达文件末尾
    bool IsEOF() const;
    
    /// 跳过空白字符
    void SkipWhitespace();
    
    /// 更新位置信息
    void UpdatePosition(char c);
    
    // === 上下文管理 ===
    
    /// 获取当前上下文
    LexerContext GetCurrentContext() const;
    
    /// 进入新上下文
    void PushContext(LexerContext context);
    
    /// 退出当前上下文
    void PopContext();
    
    /// 检测上下文变化
    void DetectContextChange(const std::string& identifier);
    
    // === Token读取方法 ===
    
    /// 读取下一个Token
    Token ReadNextToken();
    
    /// 在全局上下文中读取Token
    Token ReadGlobalToken();
    
    /// 在样式块上下文中读取Token
    Token ReadStyleToken();
    
    /// 在脚本块上下文中读取Token
    Token ReadScriptToken();
    
    /// 在文本块上下文中读取Token
    Token ReadTextToken();
    
    /// 读取标识符（支持连字符）
    Token ReadIdentifier();
    
    /// 读取CSS属性名
    Token ReadCssProperty();
    
    /// 读取CSS选择器
    Token ReadCssSelector();
    
    /// 读取CSS值
    Token ReadCssValue();
    
    /// 读取数字（支持单位）
    Token ReadNumber();
    
    /// 读取颜色值
    Token ReadColorValue();
    
    /// 读取字符串字面量
    Token ReadStringLiteral(char quote);
    
    /// 读取无修饰字面量
    Token ReadUnquotedLiteral();
    
    /// 读取百分比值
    Token ReadPercentageValue();
    
    /// 读取URL值
    Token ReadUrlValue();
    
    /// 读取注释
    Token ReadComment();
    
    /// 读取符号
    Token ReadSymbol();
    
    // === 字符分类方法 ===
    
    /// 检查是否为字母
    bool IsAlpha(char c) const;
    
    /// 检查是否为数字
    bool IsDigit(char c) const;
    
    /// 检查是否为字母数字
    bool IsAlphaNumeric(char c) const;
    
    /// 检查是否为空白字符
    bool IsWhitespace(char c) const;
    
    /// 检查是否为换行符
    bool IsNewline(char c) const;
    
    /// 检查是否为十六进制字符
    bool IsHexDigit(char c) const;
    
    /// 检查是否为CSS标识符字符
    bool IsCssIdentifierChar(char c) const;
    
    /// 检查是否为无修饰字面量字符
    bool IsUnquotedLiteralChar(char c) const;
    
    /// 检查是否为CSS值字符
    bool IsCssValueChar(char c) const;
    
    // === Token分类和验证 ===
    
    /// 检查标识符是否为关键字
    TokenType ClassifyIdentifier(const std::string& identifier) const;
    
    /// 检查是否为CSS属性
    bool IsCssProperty(const std::string& name) const;
    
    /// 检查是否为CSS选择器
    bool IsCssSelector(const std::string& value) const;
    
    /// 检查是否为颜色值
    bool IsColorValue(const std::string& value) const;
    
    /// 检查是否为CSS单位
    bool HasCssUnit(const std::string& value) const;
    
    // === 辅助方法 ===
    
    /// 添加Token到缓冲区
    void AddToken(const Token& token);
    
    /// 添加错误信息
    void AddError(const std::string& message);
    
    /// 重置分析器状态
    void Reset();
    
    /// 获取当前位置信息
    TokenPosition GetCurrentPosition() const;
    
    /// 预读字符串
    std::string PeekString(size_t length) const;
    
    /// 跳过指定数量的字符
    void SkipChars(size_t count);
    
    /// 验证Token序列
    bool ValidateTokenSequence() const;
};

} // namespace CHTL