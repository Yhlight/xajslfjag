#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <regex>
#include "Arg.h"

namespace CHTL {
namespace CJMOD {

/**
 * Syntax - CJMOD语法分析类
 * 负责对CHTL JS语法进行解析和识别
 */
class Syntax {
public:
    /**
     * 分析语法，返回一个Arg对象
     * @param pattern 语法模式字符串，如 "$ ** $"
     * @return 解析出的Arg对象，包含参数列表
     */
    static Arg analyze(const std::string& pattern);
    
    /**
     * 判断是否是JavaScript对象
     * @param code JavaScript代码
     * @return 是否为对象
     */
    static bool isObject(const std::string& code);
    
    /**
     * 判断是否是JavaScript函数
     * @param code JavaScript代码
     * @return 是否为函数
     */
    static bool isFunction(const std::string& code);
    
    /**
     * 判断是否是JavaScript数组
     * @param code JavaScript代码
     * @return 是否为数组
     */
    static bool isArray(const std::string& code);
    
    /**
     * 判断是否是CHTL JS函数
     * @param code CHTL JS代码
     * @return 是否为CHTL JS函数
     */
    static bool isCHTLJSFunction(const std::string& code);
    
    /**
     * 判断是否是CHTL JS虚对象语法
     * @param code CHTL JS代码
     * @return 是否为虚对象语法
     */
    static bool isVirtualObject(const std::string& code);
    
    /**
     * 解析函数名称
     * @param code 函数代码
     * @return 函数名称
     */
    static std::string extractFunctionName(const std::string& code);
    
    /**
     * 解析函数参数
     * @param code 函数代码
     * @return 参数列表
     */
    static std::vector<std::string> extractFunctionParameters(const std::string& code);
    
    /**
     * 解析CHTL JS函数的键值对参数
     * @param code CHTL JS函数代码
     * @return 键值对映射
     */
    static std::unordered_map<std::string, std::string> extractCHTLJSParameters(const std::string& code);
    
    /**
     * 标准化代码格式
     * @param code 原始代码
     * @return 标准化后的代码
     */
    static std::string normalize(const std::string& code);
    
    /**
     * 移除注释
     * @param code 包含注释的代码
     * @return 移除注释后的代码
     */
    static std::string removeComments(const std::string& code);
    
    /**
     * 验证语法模式
     * @param pattern 语法模式
     * @return 是否为有效模式
     */
    static bool validatePattern(const std::string& pattern);

private:
    // 静态正则表达式模式
    static const std::regex OBJECT_PATTERN;
    static const std::regex FUNCTION_PATTERN;
    static const std::regex ARRAY_PATTERN;
    static const std::regex CHTLJS_FUNCTION_PATTERN;
    static const std::regex VIRTUAL_OBJECT_PATTERN;
    static const std::regex PLACEHOLDER_PATTERN;
    static const std::regex FUNCTION_NAME_PATTERN;
    static const std::regex PARAMETER_PATTERN;
    static const std::regex KEY_VALUE_PATTERN;
    
    // 内部解析方法
    static std::vector<std::string> tokenizePattern(const std::string& pattern);
    static bool isPlaceholder(const std::string& token);
    static bool isOperator(const std::string& token);
    static bool isKeyword(const std::string& token);
    static std::string classifyToken(const std::string& token);
    
    // 语法验证方法
    static bool validateJavaScriptSyntax(const std::string& code);
    static bool validateCHTLJSSyntax(const std::string& code);
    static bool hasBalancedBraces(const std::string& code);
    static bool hasBalancedParentheses(const std::string& code);
    
    // 工具方法
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
};

/**
 * SyntaxType - 语法类型枚举
 */
enum class SyntaxType {
    UNKNOWN,
    JAVASCRIPT_OBJECT,
    JAVASCRIPT_FUNCTION,
    JAVASCRIPT_ARRAY,
    CHTLJS_FUNCTION,
    VIRTUAL_OBJECT,
    OPERATOR,
    PLACEHOLDER,
    KEYWORD,
    LITERAL
};

/**
 * SyntaxElement - 语法元素
 */
struct SyntaxElement {
    std::string content;
    SyntaxType type;
    std::vector<std::string> attributes;
    std::unordered_map<std::string, std::string> metadata;
    
    SyntaxElement() : type(SyntaxType::UNKNOWN) {}
    SyntaxElement(const std::string& content, SyntaxType type) 
        : content(content), type(type) {}
    
    bool isPlaceholder() const { return type == SyntaxType::PLACEHOLDER; }
    bool isOperator() const { return type == SyntaxType::OPERATOR; }
    bool isKeyword() const { return type == SyntaxType::KEYWORD; }
    bool isFunction() const { 
        return type == SyntaxType::JAVASCRIPT_FUNCTION || type == SyntaxType::CHTLJS_FUNCTION; 
    }
};

/**
 * SyntaxAnalyzer - 高级语法分析器
 */
class SyntaxAnalyzer {
public:
    /**
     * 深度分析语法结构
     * @param code 代码字符串
     * @return 语法元素列表
     */
    static std::vector<SyntaxElement> deepAnalyze(const std::string& code);
    
    /**
     * 构建语法树
     * @param elements 语法元素列表
     * @return 根语法元素
     */
    static SyntaxElement buildSyntaxTree(const std::vector<SyntaxElement>& elements);
    
    /**
     * 语法转换
     * @param source 源语法元素
     * @param target_type 目标类型
     * @return 转换后的语法元素
     */
    static SyntaxElement transform(const SyntaxElement& source, SyntaxType target_type);
    
    /**
     * 语法优化
     * @param elements 语法元素列表
     * @return 优化后的语法元素列表
     */
    static std::vector<SyntaxElement> optimize(const std::vector<SyntaxElement>& elements);

private:
    static SyntaxType detectType(const std::string& content);
    static std::vector<std::string> extractAttributes(const std::string& content, SyntaxType type);
    static std::unordered_map<std::string, std::string> extractMetadata(const std::string& content);
};

/**
 * SyntaxPattern - 语法模式匹配器
 */
class SyntaxPattern {
public:
    /**
     * 添加自定义模式
     * @param name 模式名称
     * @param pattern 正则表达式模式
     * @param type 语法类型
     */
    static void addPattern(const std::string& name, const std::string& pattern, SyntaxType type);
    
    /**
     * 移除模式
     * @param name 模式名称
     */
    static void removePattern(const std::string& name);
    
    /**
     * 匹配模式
     * @param code 代码
     * @param pattern_name 模式名称
     * @return 是否匹配
     */
    static bool matches(const std::string& code, const std::string& pattern_name);
    
    /**
     * 获取所有匹配的模式
     * @param code 代码
     * @return 匹配的模式名称列表
     */
    static std::vector<std::string> getMatchingPatterns(const std::string& code);

private:
    static std::unordered_map<std::string, std::pair<std::regex, SyntaxType>> custom_patterns;
};

} // namespace CJMOD
} // namespace CHTL