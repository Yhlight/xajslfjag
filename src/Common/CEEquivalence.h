#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {
namespace Common {

// CE对等式上下文类型
enum class CEContext {
    ATTRIBUTE_ASSIGNMENT,   // 属性赋值：id: box 或 id = box
    STYLE_PROPERTY,         // 样式属性：color: red 或 color = red
    VARIABLE_ASSIGNMENT,    // 变量赋值：变量名: 值 或 变量名 = 值
    TEMPLATE_USAGE,         // 模板使用：@Style Template: 或 @Style Template =
    CONFIGURATION,          // 配置项：INDEX_INITIAL_COUNT: 0 或 INDEX_INITIAL_COUNT = 0
    KEY_VALUE_PAIR,         // 键值对：key: value 或 key = value
    FUNCTION_PARAMETER,     // 函数参数：param: value 或 param = value
    UNKNOWN
};

// CE对等式处理结果
struct CEProcessResult {
    std::string processed_content;
    std::vector<std::pair<size_t, char>> substitutions;  // 位置和替换的字符
    bool has_changes;
    
    CEProcessResult() : has_changes(false) {}
    
    void addSubstitution(size_t pos, char original, char replacement) {
        substitutions.emplace_back(pos, replacement);
        has_changes = true;
    }
};

// CE对等式检测结果
struct CEDetectionResult {
    CEContext context;
    size_t position;
    char symbol;            // ':' 或 '='
    char recommended;       // 推荐使用的符号
    std::string explanation; // 说明为什么推荐这个符号
    
    CEDetectionResult() : context(CEContext::UNKNOWN), position(0), symbol('\0'), recommended('\0') {}
    
    CEDetectionResult(CEContext ctx, size_t pos, char sym)
        : context(ctx), position(pos), symbol(sym), recommended('\0') {}
    
    bool hasRecommendation() const { return recommended != '\0'; }
};

// CE对等式管理器
class CEEquivalence {
public:
    CEEquivalence();
    ~CEEquivalence();
    
    // 配置
    void setStrictMode(bool strict) { strict_mode = strict; }
    void setPreferredSymbol(CEContext context, char symbol);
    void setDefaultPreferredSymbol(char symbol) { default_preferred = symbol; }
    
    // 核心功能
    
    // 规范化CE符号（将'='转换为':'或相反）
    CEProcessResult normalizeCESymbols(const std::string& content, 
                                       CEContext context = CEContext::UNKNOWN);
    
    // 检测CE符号使用情况
    std::vector<CEDetectionResult> detectCEUsage(const std::string& content);
    
    // 验证CE符号等价性
    bool validateEquivalence(const std::string& content);
    
    // 推荐最佳CE符号
    char recommendSymbol(CEContext context, const std::string& content, size_t position);
    
    // 上下文检测
    
    // 检测当前位置的CE上下文
    CEContext detectContext(const std::string& content, size_t position);
    
    // 检查是否在属性赋值上下文中
    bool isAttributeContext(const std::string& content, size_t position);
    
    // 检查是否在样式属性上下文中  
    bool isStylePropertyContext(const std::string& content, size_t position);
    
    // 检查是否在变量赋值上下文中
    bool isVariableContext(const std::string& content, size_t position);
    
    // 检查是否在配置上下文中
    bool isConfigurationContext(const std::string& content, size_t position);
    
    // 符号处理
    
    // 检查字符是否是CE符号
    bool isCESymbol(char c) const { return c == ':' || c == '='; }
    
    // 获取CE符号的等价符号
    char getEquivalent(char symbol) const {
        return (symbol == ':') ? '=' : (symbol == '=' ? ':' : symbol);
    }
    
    // 替换CE符号
    std::string replaceCESymbol(const std::string& content, size_t position, char new_symbol);
    
    // 查找所有CE符号位置
    std::vector<size_t> findCEPositions(const std::string& content);
    
    // 验证和修正
    
    // 验证CE符号使用的一致性
    bool validateConsistency(const std::string& content, CEContext context);
    
    // 修正CE符号使用
    std::string correctCEUsage(const std::string& content, CEContext context = CEContext::UNKNOWN);
    
    // 检查CE符号周围的语法
    bool isValidCESyntax(const std::string& content, size_t position);
    
    // 推荐规则
    
    // 获取推荐的CE符号（基于上下文）
    char getRecommendedSymbol(CEContext context) const;
    
    // 设置推荐规则
    void setRecommendationRule(CEContext context, char symbol, const std::string& reason);
    
    // 获取推荐理由
    std::string getRecommendationReason(CEContext context) const;
    
    // 统计和分析
    
    // 统计CE符号使用情况
    std::unordered_map<char, size_t> getCEUsageStatistics(const std::string& content);
    
    // 分析CE符号分布
    std::unordered_map<CEContext, std::unordered_map<char, size_t>> analyzeCEDistribution(
        const std::string& content);
    
    // 检查混合使用情况
    bool hasMixedUsage(const std::string& content, CEContext context = CEContext::UNKNOWN);
    
    // 调试和诊断
    void dumpCEAnalysis(const std::string& content);
    std::vector<std::string> getCEWarnings(const std::string& content);
    
    // 预设配置
    
    // 应用CHTL标准配置
    void applyCHTLStandardConfig();
    
    // 应用严格一致性配置
    void applyStrictConsistencyConfig();
    
    // 应用推荐配置
    void applyRecommendedConfig();

private:
    bool strict_mode;
    char default_preferred;
    std::unordered_map<CEContext, char> preferred_symbols;
    std::unordered_map<CEContext, std::string> recommendation_reasons;
    
    // 内部方法
    
    // 上下文分析
    CEContext analyzeContext(const std::string& content, size_t position);
    bool isInAttributeBlock(const std::string& content, size_t position);
    bool isInStyleBlock(const std::string& content, size_t position);
    bool isInConfigBlock(const std::string& content, size_t position);
    bool isInTemplateUsage(const std::string& content, size_t position);
    
    // 语法分析
    std::pair<size_t, size_t> findSurroundingContext(const std::string& content, size_t position);
    std::string extractContextualInfo(const std::string& content, size_t start, size_t end);
    bool isValidIdentifier(const std::string& str);
    
    // 符号查找
    size_t findNextCESymbol(const std::string& content, size_t start);
    size_t findPrevCESymbol(const std::string& content, size_t start);
    std::vector<size_t> findAllCEInContext(const std::string& content, CEContext context);
    
    // 验证逻辑
    bool isContextuallyValid(const std::string& content, size_t position, char symbol);
    bool checkSurroundingSyntax(const std::string& content, size_t position);
    
    // 工具方法
    std::string getContextName(CEContext context) const;
    bool isWhitespace(char c) const;
    bool isAlphaNumeric(char c) const;
    size_t skipWhitespace(const std::string& content, size_t position, bool forward = true);
    
    // 块检测辅助
    std::pair<size_t, size_t> findEnclosingBraces(const std::string& content, size_t position);
    bool isInQuotes(const std::string& content, size_t position);
    bool isInComment(const std::string& content, size_t position);
};

// CE对等式工具类
class CEUtils {
public:
    // 快速CE符号替换
    static std::string quickReplace(const std::string& content, char from, char to);
    
    // 智能CE符号推荐
    static char smartRecommend(const std::string& line, size_t position);
    
    // CE符号一致性检查
    static bool isConsistent(const std::string& content);
    
    // 获取CE符号使用摘要
    static std::string getUsageSummary(const std::string& content);
    
    // CE符号规范化
    static std::string normalize(const std::string& content, char preferred = ':');
    
    // 检查是否符合CHTL风格指南
    static bool followsCHTLStyle(const std::string& content);
    
    // 应用CHTL风格指南
    static std::string applyCHTLStyle(const std::string& content);
    
    // 验证CE语法
    static std::vector<std::string> validateSyntax(const std::string& content);
};

} // namespace Common
} // namespace CHTL