#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Core/Common.h"
#include "CHTL/Token.h"
#include "Scanner/UnifiedScanner.h"

namespace CHTL {

// 词法分析器配置
struct LexerConfig {
    bool skipWhitespace = true;       // 是否跳过空白
    bool skipComments = false;        // 是否跳过注释
    bool preserveLineInfo = true;     // 是否保留行信息
    bool enableCEEquivalence = true;  // 是否启用CE对等式
    bool allowUnquotedLiterals = true; // 是否允许无修饰字面量
};

// CHTL词法分析器
class CHTLLexer {
private:
    LexerConfig config_;
    TokenStream tokenStream_;
    String currentFile_;
    Position currentPosition_;
    
    // 错误处理
    Vector<String> errors_;
    
public:
    CHTLLexer();
    explicit CHTLLexer(const LexerConfig& config);
    ~CHTLLexer() = default;
    
    // 主要接口：从扫描器切片生成Token流
    Result<TokenStream> Tokenize(const Vector<ScannerSlice>& slices);
    
    // 从字符串直接生成Token流（用于测试）
    Result<TokenStream> TokenizeString(const String& input, const String& filename = "<input>");
    
    // 配置管理
    void SetConfig(const LexerConfig& config) { config_ = config; }
    const LexerConfig& GetConfig() const { return config_; }
    
    // 获取错误
    Vector<String> GetErrors() const { return errors_; }
    bool HasErrors() const { return !errors_.empty(); }
    
    // 清空状态
    void Reset();
    
private:
    // 处理不同类型的切片
    Result<void> ProcessSlice(const ScannerSlice& slice);
    
    // CHTL特定切片处理
    Result<void> ProcessCHTLElement(const ScannerSlice& slice);
    Result<void> ProcessCHTLAttribute(const ScannerSlice& slice);
    Result<void> ProcessCHTLText(const ScannerSlice& slice);
    Result<void> ProcessCHTLStyle(const ScannerSlice& slice);
    Result<void> ProcessCHTLScript(const ScannerSlice& slice);
    Result<void> ProcessCHTLTemplate(const ScannerSlice& slice);
    Result<void> ProcessCHTLCustom(const ScannerSlice& slice);
    Result<void> ProcessCHTLOrigin(const ScannerSlice& slice);
    Result<void> ProcessCHTLImport(const ScannerSlice& slice);
    Result<void> ProcessCHTLNamespace(const ScannerSlice& slice);
    Result<void> ProcessCHTLConfiguration(const ScannerSlice& slice);
    
    // 通用切片处理
    Result<void> ProcessPlainCSS(const ScannerSlice& slice);
    Result<void> ProcessPlainJavaScript(const ScannerSlice& slice);
    Result<void> ProcessComment(const ScannerSlice& slice);
    Result<void> ProcessWhitespace(const ScannerSlice& slice);
    
    // Token化辅助函数
    Result<void> TokenizeContent(const String& content, const SourceLocation& location);
    Result<void> TokenizeBlockMarker(const String& marker, const SourceLocation& location);
    Result<void> TokenizeAtTypeMarker(const String& marker, const SourceLocation& location);
    Result<void> TokenizeKeyword(const String& keyword, const SourceLocation& location);
    Result<void> TokenizeIdentifier(const String& identifier, const SourceLocation& location);
    Result<void> TokenizeLiteral(const String& literal, const SourceLocation& location);
    Result<void> TokenizeOperator(char op, const SourceLocation& location);
    Result<void> TokenizeString(const String& str, const SourceLocation& location);
    
    // 字符串处理
    Result<String> ParseQuotedString(const String& str);
    Result<String> ParseUnquotedLiteral(const String& str);
    
    // 判断函数
    bool IsBlockMarker(const String& str) const;
    bool IsAtTypeMarker(const String& str) const;
    bool IsKeyword(const String& str) const;
    bool IsOperator(char ch) const;
    bool IsIdentifierStart(char ch) const;
    bool IsIdentifierPart(char ch) const;
    
    // 字符串扫描辅助
    struct StringScanner {
        const String& str;
        size_t pos = 0;
        Position position;
        
        StringScanner(const String& s, const Position& p) 
            : str(s), position(p) {}
        
        bool IsAtEnd() const { return pos >= str.length(); }
        char Current() const { return IsAtEnd() ? '\0' : str[pos]; }
        char Peek(size_t offset = 1) const { 
            return pos + offset >= str.length() ? '\0' : str[pos + offset]; 
        }
        void Advance();
        void SkipWhitespace();
        String ReadWhile(std::function<bool(char)> predicate);
        String ReadUntil(char delimiter);
        String ReadUntilAny(const String& delimiters);
    };
    
    // 创建Token并添加到流
    void AddToken(TokenType type, const String& value, const SourceLocation& location);
    
    // 错误处理
    void AddError(const String& error, const SourceLocation& location);
};

// 词法分析器工厂
class LexerFactory {
public:
    static UniquePtr<CHTLLexer> CreateLexer(const LexerConfig& config = {});
};

// Token验证器
class TokenValidator {
public:
    // 验证Token流是否符合CHTL语法规则
    static Result<void> ValidateTokenStream(const TokenStream& stream);
    
    // 验证特定上下文中的Token
    static bool IsValidInContext(const Token& token, CompileState state);
    
    // 验证属性语法
    static bool IsValidAttribute(const Token& nameToken, const Token& valueToken);
    
    // 验证选择器语法
    static bool IsValidSelector(const Token& token);
    
    // 验证配置键值对
    static bool IsValidConfigPair(const Token& keyToken, const Token& valueToken);
};

// 词法分析工具类
class LexerUtils {
public:
    // 转义字符串中的特殊字符
    static String EscapeString(const String& str);
    
    // 反转义字符串
    static String UnescapeString(const String& str);
    
    // 规范化标识符（处理-和_）
    static String NormalizeIdentifier(const String& identifier);
    
    // 检查是否是有效的HTML标签名
    static bool IsValidHTMLTag(const String& tag);
    
    // 检查是否是有效的CSS属性名
    static bool IsValidCSSProperty(const String& property);
    
    // 提取@类型标记的类型部分
    static String ExtractAtType(const String& marker);
    
    // 提取块标记的类型部分
    static String ExtractBlockType(const String& marker);
};

} // namespace CHTL

#endif // CHTL_LEXER_H