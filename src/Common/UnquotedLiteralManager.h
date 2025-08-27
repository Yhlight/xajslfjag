#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <regex>
#include <memory>

namespace CHTL {
namespace Common {

// 无修饰字面量类型
enum class UnquotedLiteralType {
    TEXT_CONTENT,           // text节点内容
    ATTRIBUTE_VALUE,        // 属性值
    CSS_PROPERTY_VALUE,     // CSS属性值
    STYLE_SELECTOR,         // 样式选择器
    UNKNOWN
};

// 无修饰字面量上下文
enum class UnquotedLiteralContext {
    TEXT_BLOCK,             // text { } 块内
    ELEMENT_ATTRIBUTE,      // 元素属性
    STYLE_PROPERTY,         // style块中的CSS属性
    STYLE_SELECTOR_RULE,    // style块中的选择器
    SCRIPT_BLOCK,           // script块（有限支持）
    UNKNOWN_CONTEXT
};

// 无修饰字面量验证结果
struct UnquotedValidationResult {
    bool is_valid;                          // 是否有效
    UnquotedLiteralType detected_type;      // 检测到的类型
    std::string normalized_value;           // 标准化后的值
    std::vector<std::string> warnings;     // 警告信息
    std::vector<std::string> errors;       // 错误信息
    
    UnquotedValidationResult() : is_valid(false), detected_type(UnquotedLiteralType::UNKNOWN) {}
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    void addError(const std::string& error) {
        errors.push_back(error);
        is_valid = false;
    }
    
    bool hasWarnings() const { return !warnings.empty(); }
    bool hasErrors() const { return !errors.empty(); }
};

// 无修饰字面量管理器
class UnquotedLiteralManager {
public:
    UnquotedLiteralManager();
    ~UnquotedLiteralManager();
    
    // 验证无修饰字面量
    UnquotedValidationResult validateUnquotedLiteral(const std::string& content, 
                                                     UnquotedLiteralContext context);
    
    // 检测是否为有效的无修饰字面量
    bool isValidUnquotedLiteral(const std::string& content, 
                               UnquotedLiteralContext context);
    
    // 标准化无修饰字面量
    std::string normalizeUnquotedLiteral(const std::string& content, 
                                        UnquotedLiteralType type);
    
    // 转换为带引号的字符串
    std::string toQuotedString(const std::string& unquoted_content, 
                              UnquotedLiteralType type);
    
    // 从带引号字符串提取内容
    std::string fromQuotedString(const std::string& quoted_content);
    
    // 检测字面量类型
    UnquotedLiteralType detectLiteralType(const std::string& content, 
                                         UnquotedLiteralContext context);
    
    // 配置选项
    void setStrictMode(bool strict) { strict_mode = strict; }
    void setAllowSpecialCharacters(bool allow) { allow_special_chars = allow; }
    void setCSSCompatibleMode(bool css_mode) { css_compatible_mode = css_mode; }
    
    // 获取支持的上下文
    std::vector<UnquotedLiteralContext> getSupportedContexts() const;
    
    // 获取上下文说明
    std::string getContextDescription(UnquotedLiteralContext context) const;

private:
    // 配置选项
    bool strict_mode;
    bool allow_special_chars;
    bool css_compatible_mode;
    
    // 预定义模式
    std::unordered_map<UnquotedLiteralContext, std::vector<std::regex>> validation_patterns;
    std::unordered_map<UnquotedLiteralType, std::regex> normalization_patterns;
    
    // 保留字和关键字
    std::unordered_set<std::string> reserved_keywords;
    std::unordered_set<std::string> html_keywords;
    std::unordered_set<std::string> css_keywords;
    std::unordered_set<std::string> chtl_keywords;
    
    // 特殊字符集
    std::unordered_set<char> forbidden_chars;
    std::unordered_set<char> special_chars;
    std::unordered_set<char> css_allowed_chars;
    
    // 初始化方法
    void initializeValidationPatterns();
    void initializeReservedKeywords();
    void initializeCharacterSets();
    
    // 验证方法
    bool validateTextContent(const std::string& content);
    bool validateAttributeValue(const std::string& content);
    bool validateCSSPropertyValue(const std::string& content);
    bool validateStyleSelector(const std::string& content);
    
    // 特殊验证
    bool containsForbiddenCharacters(const std::string& content);
    bool isReservedKeyword(const std::string& content, UnquotedLiteralContext context);
    bool hasValidCSSSyntax(const std::string& content);
    
    // 标准化方法
    std::string normalizeTextContent(const std::string& content);
    std::string normalizeAttributeValue(const std::string& content);
    std::string normalizeCSSValue(const std::string& content);
    
    // 工具方法
    std::string trimWhitespace(const std::string& content);
    std::string escapeSpecialCharacters(const std::string& content, UnquotedLiteralType type);
    std::string unescapeSpecialCharacters(const std::string& content);
    
    // 上下文检测
    UnquotedLiteralContext detectCurrentContext(const std::string& surrounding_code, 
                                               size_t position);
    
    // 错误处理
    void reportValidationError(UnquotedValidationResult& result, 
                              const std::string& error);
    void reportValidationWarning(UnquotedValidationResult& result, 
                                const std::string& warning);
};

// 无修饰字面量处理工具类
class UnquotedLiteralUtils {
public:
    // 检测字符串是否需要引号
    static bool needsQuotes(const std::string& content, UnquotedLiteralContext context);
    
    // 智能添加引号
    static std::string smartQuote(const std::string& content, UnquotedLiteralContext context);
    
    // 智能移除引号
    static std::string smartUnquote(const std::string& content);
    
    // 检测引号类型
    enum class QuoteType { NONE, SINGLE, DOUBLE, BACKTICK };
    static QuoteType detectQuoteType(const std::string& content);
    
    // 转换引号类型
    static std::string convertQuoteType(const std::string& content, QuoteType target_type);
    
    // 验证CSS兼容性
    static bool isCSSCompatible(const std::string& content);
    
    // 生成HTML属性
    static std::string generateHTMLAttribute(const std::string& name, 
                                           const std::string& value, 
                                           bool force_quotes = false);
    
    // 生成CSS属性
    static std::string generateCSSProperty(const std::string& property, 
                                         const std::string& value);
    
    // 文本内容处理
    static std::string processTextContent(const std::string& content, 
                                        bool preserve_whitespace = false);
    
    // 验证标识符
    static bool isValidIdentifier(const std::string& content);
    static bool isValidCSSIdentifier(const std::string& content);
    static bool isValidHTMLAttributeName(const std::string& content);
    
    // 字符分类
    static bool isWhitespace(char c);
    static bool isAlphanumeric(char c);
    static bool isCSSSpecialChar(char c);
    static bool isHTMLSpecialChar(char c);
    
    // 错误检测
    static std::vector<std::string> detectPotentialIssues(const std::string& content, 
                                                          UnquotedLiteralContext context);
    
    // 自动修正建议
    static std::vector<std::string> suggestCorrections(const std::string& content, 
                                                       UnquotedLiteralContext context);
    
    // 性能优化
    static bool fastValidate(const std::string& content, UnquotedLiteralContext context);
    
    // 调试工具
    static std::string analyzeContent(const std::string& content, 
                                    UnquotedLiteralContext context);
    static void dumpCharacterAnalysis(const std::string& content);
};

// 无修饰字面量解析器
class UnquotedLiteralParser {
public:
    UnquotedLiteralParser();
    ~UnquotedLiteralParser();
    
    // 解析无修饰字面量
    struct ParseResult {
        std::string content;                    // 解析后的内容
        UnquotedLiteralType type;              // 检测到的类型
        size_t start_position;                 // 开始位置
        size_t end_position;                   // 结束位置
        bool success;                          // 是否成功
        std::vector<std::string> errors;       // 错误信息
        
        ParseResult() : type(UnquotedLiteralType::UNKNOWN), 
                       start_position(0), end_position(0), success(false) {}
    };
    
    // 从源码中解析无修饰字面量
    ParseResult parseFromSource(const std::string& source, 
                               size_t start_pos, 
                               UnquotedLiteralContext context);
    
    // 解析text块内容
    ParseResult parseTextContent(const std::string& source, size_t start_pos);
    
    // 解析属性值
    ParseResult parseAttributeValue(const std::string& source, size_t start_pos);
    
    // 解析CSS值
    ParseResult parseCSSValue(const std::string& source, size_t start_pos);
    
    // 配置解析器
    void setStrictMode(bool strict) { strict_mode = strict; }
    void setMaxLength(size_t max_len) { max_length = max_len; }
    
private:
    bool strict_mode;
    size_t max_length;
    
    // 解析辅助方法
    bool isValidStartChar(char c, UnquotedLiteralContext context);
    bool isValidContentChar(char c, UnquotedLiteralContext context);
    bool isTerminatorChar(char c, UnquotedLiteralContext context);
    
    size_t findContentEnd(const std::string& source, 
                         size_t start_pos, 
                         UnquotedLiteralContext context);
    
    std::string extractContent(const std::string& source, 
                              size_t start_pos, 
                              size_t end_pos);
};

} // namespace Common
} // namespace CHTL