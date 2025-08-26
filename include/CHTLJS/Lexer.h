#ifndef CHTLJS_LEXER_H
#define CHTLJS_LEXER_H

#include "Core/Common.h"
#include "CHTLJS/Token.h"
#include "Scanner/UnifiedScanner.h"

namespace CHTLJS {

// CHTL JS词法分析器配置
struct LexerConfig {
    bool skipWhitespace = true;         // 是否跳过空白
    bool skipComments = false;          // 是否跳过注释
    bool preserveLineInfo = true;       // 是否保留行信息
    bool allowUnquotedLiterals = true;  // 是否允许无修饰字面量
    bool supportAllEvents = true;       // 是否支持所有JS事件
    bool enableKeyValueUnordered = true; // 是否启用键值对无序
    bool enableOptionalValues = true;    // 是否启用可选值
};

// CHTL JS词法分析器
class CHTLJSLexer {
private:
    LexerConfig config_;
    TokenStream tokenStream_;
    String currentFile_;
    Position currentPosition_;
    
    // 错误处理
    Vector<String> errors_;
    
    // 增强选择器状态
    struct {
        bool inSelector = false;
        int depth = 0;
        String currentSelector;
    } selectorState_;
    
    // 箭头操作符状态
    struct {
        bool expectingArrow = false;
        Token lastToken;
    } arrowState_;
    
public:
    CHTLJSLexer();
    explicit CHTLJSLexer(const LexerConfig& config);
    ~CHTLJSLexer() = default;
    
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
    
    // CHTL JS特定切片处理
    Result<void> ProcessCHTLJSEnhancedSelector(const ScannerSlice& slice);
    Result<void> ProcessCHTLJSArrowOperator(const ScannerSlice& slice);
    Result<void> ProcessCHTLJSListen(const ScannerSlice& slice);
    Result<void> ProcessCHTLJSDelegate(const ScannerSlice& slice);
    Result<void> ProcessCHTLJSAnimate(const ScannerSlice& slice);
    Result<void> ProcessCHTLJSVir(const ScannerSlice& slice);
    
    // JavaScript切片处理
    Result<void> ProcessJavaScript(const ScannerSlice& slice);
    Result<void> ProcessJavaScriptFunction(const ScannerSlice& slice);
    Result<void> ProcessJavaScriptObject(const ScannerSlice& slice);
    Result<void> ProcessJavaScriptArray(const ScannerSlice& slice);
    
    // Token化辅助函数
    Result<void> TokenizeContent(const String& content, const SourceLocation& location);
    Result<void> TokenizeEnhancedSelector(const String& selector, const SourceLocation& location);
    Result<void> TokenizeArrowFunction(const String& params, const String& body, const SourceLocation& location);
    Result<void> TokenizeEventListener(const String& content, const SourceLocation& location);
    Result<void> TokenizeAnimation(const String& content, const SourceLocation& location);
    Result<void> TokenizeVirtualObject(const String& content, const SourceLocation& location);
    Result<void> TokenizeKeyValuePairs(const String& content, const SourceLocation& location, bool allowUnordered);
    
    // 特殊处理
    Result<void> ProcessUnquotedLiteral(const String& literal, const SourceLocation& location);
    Result<void> ProcessEventName(const String& eventName, const SourceLocation& location);
    Result<void> ProcessOptionalValue(const String& key, const String& value, const SourceLocation& location);
    
    // 判断函数
    bool IsEnhancedSelectorStart(const String& str) const;
    bool IsArrowOperator(const String& str) const;
    bool IsCHTLJSKeyword(const String& str) const;
    bool IsEventName(const String& str) const;
    bool IsUnquotedLiteral(const String& str) const;
    
    // 字符串扫描辅助
    class StringScanner {
    private:
        const String& str_;
        size_t pos_ = 0;
        Position position_;
        
    public:
        StringScanner(const String& s, const Position& p) 
            : str_(s), position_(p) {}
        
        bool IsAtEnd() const { return pos_ >= str_.length(); }
        char Current() const { return IsAtEnd() ? '\0' : str_[pos_]; }
        char Peek(size_t offset = 1) const { 
            return pos_ + offset >= str_.length() ? '\0' : str_[pos_ + offset]; 
        }
        
        void Advance();
        void SkipWhitespace();
        String ReadWhile(std::function<bool(char)> predicate);
        String ReadUntil(char delimiter);
        String ReadUntilAny(const String& delimiters);
        String ReadBalanced(char open, char close);
        
        // CHTL JS特有方法
        bool MatchDoubleChar(char ch);  // 匹配 {{ 或 }}
        String ReadEnhancedSelector();   // 读取增强选择器内容
        String ReadArrowBody();          // 读取箭头函数体
        
        Position GetPosition() const { return position_; }
    };
    
    // 创建Token并添加到流
    void AddToken(TokenType type, const String& value, const SourceLocation& location);
    
    // 错误处理
    void AddError(const String& error, const SourceLocation& location);
    
    // 增强选择器处理
    void HandleEnhancedSelectorStart();
    void HandleEnhancedSelectorEnd();
    
    // 箭头操作符处理
    void HandleArrowOperator(const Token& leftOperand);
};

// 词法分析器工厂
class LexerFactory {
public:
    static UniquePtr<CHTLJSLexer> CreateLexer(const LexerConfig& config = {});
};

// CHTL JS Token验证器
class TokenValidator {
public:
    // 验证Token流是否符合CHTL JS语法规则
    static Result<void> ValidateTokenStream(const TokenStream& stream);
    
    // 验证增强选择器
    static bool IsValidEnhancedSelector(const TokenStream& stream, size_t start, size_t end);
    
    // 验证箭头操作符使用
    static bool IsValidArrowUsage(const Token& left, const Token& arrow, const Token& right);
    
    // 验证事件监听器
    static bool IsValidListenStatement(const TokenStream& stream, size_t pos);
    static bool IsValidDelegateStatement(const TokenStream& stream, size_t pos);
    
    // 验证动画定义
    static bool IsValidAnimateStatement(const TokenStream& stream, size_t pos);
    
    // 验证虚对象
    static bool IsValidVirDefinition(const TokenStream& stream, size_t pos);
    
    // 验证键值对（支持无序和可选）
    static bool IsValidKeyValuePairs(const Vector<Token>& tokens, bool allowUnordered, bool allowOptional);
    
    // 验证事件名（支持所有JS事件）
    static bool IsValidEventName(const String& eventName);
};

// 词法分析工具类
class LexerUtils {
public:
    // 转换增强选择器到标准CSS选择器
    static String CompileEnhancedSelector(const String& selector);
    
    // 转换箭头函数到JavaScript函数
    static String CompileArrowFunction(const String& params, const String& body);
    
    // 处理无修饰字面量
    static String ProcessUnquotedLiteral(const String& literal);
    
    // 标准化事件名
    static String NormalizeEventName(const String& eventName);
    
    // 生成唯一的函数名（用于箭头函数）
    static String GenerateUniqueFunctionName(const String& prefix = "_arrow");
    
    // 提取键值对（支持无序）
    static HashMap<String, String> ExtractKeyValuePairs(const Vector<Token>& tokens, bool allowUnordered);
    
    // 检查是否是有效的动画属性
    static bool IsValidAnimationProperty(const String& property);
    
    // 检查是否是有效的缓动函数
    static bool IsValidEasingFunction(const String& easing);
};

} // namespace CHTLJS

#endif // CHTLJS_LEXER_H