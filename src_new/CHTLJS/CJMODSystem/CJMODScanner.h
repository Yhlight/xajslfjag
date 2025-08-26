#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Arg.h"
// 自包含，无需外部CJMODScanner依赖

namespace CHTL {
namespace CJMOD {

/**
 * CJMODScanner - CJMOD API的统一扫描器接口
 * 提供CJMOD API所需的扫描功能
 */
class CJMODScanner {
public:
    /**
     * 扫描语法片段
     * @param args 参数模式
     * @param keyword 扫描的关键字
     * @return 扫描结果Arg对象
     */
    static Arg scan(const Arg& args, const std::string& keyword = "");
    
    /**
     * 扫描语法片段（重载）
     * @param args 参数模式
     * @param keywords 多个关键字
     * @return 扫描结果Arg对象
     */
    static Arg scan(const Arg& args, const std::vector<std::string>& keywords);
    
    /**
     * 扫描代码片段
     * @param code 代码片段
     * @param pattern 扫描模式
     * @return 扫描结果Arg对象
     */
    static Arg scanCode(const std::string& code, const std::string& pattern);
    
    /**
     * 扫描CHTL JS函数调用
     * @param function_call CHTL JS函数调用代码
     * @param args 参数模式
     * @return 扫描结果Arg对象
     */
    static Arg scanCHTLJSFunction(const std::string& function_call, const Arg& args);
    
    /**
     * 扫描表达式
     * @param expression 表达式
     * @param operators 操作符列表
     * @return 扫描结果Arg对象
     */
    static Arg scanExpression(const std::string& expression, const std::vector<std::string>& operators);
    
    /**
     * 扫描虚对象语法
     * @param vir_syntax 虚对象语法
     * @param args 参数模式
     * @return 扫描结果Arg对象
     */
    static Arg scanVirtualObject(const std::string& vir_syntax, const Arg& args);
    
    /**
     * 高级扫描 - 基于上下文的智能扫描
     * @param content 内容
     * @param context 上下文信息
     * @return 扫描结果
     */
    static Arg advancedScan(const std::string& content, const ScanContext& context);

private:
    // 内部扫描方法
    static Arg performPatternScan(const std::string& content, const Arg& pattern);
    static Arg performKeywordScan(const std::string& content, const std::string& keyword);
    static Arg performExpressionScan(const std::string& content, const std::vector<std::string>& operators);
    static Arg performFunctionScan(const std::string& content, const Arg& function_pattern);
    
    // 结果处理方法
    static Arg processPatternMatches(const std::vector<std::string>& matches, const Arg& pattern);
    static Arg processKeywordMatches(const std::vector<std::string>& matches, const std::string& keyword);
    static Arg processFunctionMatches(const std::unordered_map<std::string, std::string>& matches, const Arg& pattern);
    
    // 工具方法
    static std::vector<std::string> extractMatches(const std::string& content, const std::string& pattern);
    static std::string normalizeContent(const std::string& content);
    static bool validateScanResult(const Arg& result);
};

/**
 * ScanContext - 扫描上下文
 */
struct ScanContext {
    std::string source_type;           // 源类型：CHTL, CHTLJS, CSS, JavaScript
    std::string function_name;         // 函数名
    std::unordered_map<std::string, std::string> variables; // 变量映射
    std::vector<std::string> operators;      // 操作符列表
    bool enable_virtual_object;       // 是否启用虚对象扫描
    bool enable_expression_analysis;  // 是否启用表达式分析
    bool strict_mode;                  // 严格模式
    
    ScanContext() 
        : enable_virtual_object(true)
        , enable_expression_analysis(true)
        , strict_mode(false) {}
};

/**
 * ScanStrategy - 扫描策略
 */
enum class ScanStrategy {
    PATTERN_MATCH,      // 模式匹配
    KEYWORD_SEARCH,     // 关键字搜索
    EXPRESSION_PARSE,   // 表达式解析
    FUNCTION_ANALYZE,   // 函数分析
    VIRTUAL_OBJECT,     // 虚对象处理
    HYBRID             // 混合策略
};

/**
 * ScanResult - 扫描结果详情
 */
struct ScanResult {
    Arg result_args;                              // 结果参数
    std::vector<std::string> matched_patterns;    // 匹配的模式
    std::vector<std::string> found_keywords;      // 找到的关键字
    std::unordered_map<std::string, std::string> extracted_data; // 提取的数据
    std::vector<std::string> warnings;            // 警告信息
    std::vector<std::string> errors;              // 错误信息
    bool success;                                 // 是否成功
    
    ScanResult() : success(false) {}
};

/**
 * AdvancedCJMODScanner - 高级CJMOD扫描器
 */
class AdvancedCJMODScanner {
public:
    /**
     * 设置扫描策略
     * @param strategy 扫描策略
     */
    static void setStrategy(ScanStrategy strategy);
    
    /**
     * 获取当前扫描策略
     * @return 扫描策略
     */
    static ScanStrategy getStrategy();
    
    /**
     * 执行详细扫描
     * @param content 内容
     * @param pattern 模式
     * @param context 上下文
     * @return 详细扫描结果
     */
    static ScanResult detailedScan(const std::string& content, const Arg& pattern, const ScanContext& context);
    
    /**
     * 批量扫描
     * @param contents 内容列表
     * @param pattern 模式
     * @return 扫描结果列表
     */
    static std::vector<ScanResult> batchScan(const std::vector<std::string>& contents, const Arg& pattern);
    
    /**
     * 增量扫描
     * @param previous_result 之前的结果
     * @param new_content 新内容
     * @param pattern 模式
     * @return 增量扫描结果
     */
    static ScanResult incrementalScan(const ScanResult& previous_result, const std::string& new_content, const Arg& pattern);
    
    /**
     * 添加自定义扫描器
     * @param name 扫描器名称
     * @param scanner 扫描器函数
     */
    static void addCustomScanner(const std::string& name, std::function<Arg(const std::string&, const Arg&)> scanner);
    
    /**
     * 移除自定义扫描器
     * @param name 扫描器名称
     */
    static void removeCustomScanner(const std::string& name);
    
    /**
     * 使用自定义扫描器
     * @param name 扫描器名称
     * @param content 内容
     * @param pattern 模式
     * @return 扫描结果
     */
    static Arg useCustomScanner(const std::string& name, const std::string& content, const Arg& pattern);

private:
    static ScanStrategy current_strategy;
    static std::unordered_map<std::string, std::function<Arg(const std::string&, const Arg&)>> custom_scanners;
    
    // 策略实现方法
    static ScanResult patternMatchScan(const std::string& content, const Arg& pattern, const ScanContext& context);
    static ScanResult keywordSearchScan(const std::string& content, const Arg& pattern, const ScanContext& context);
    static ScanResult expressionParseScan(const std::string& content, const Arg& pattern, const ScanContext& context);
    static ScanResult functionAnalyzeScan(const std::string& content, const Arg& pattern, const ScanContext& context);
    static ScanResult virtualObjectScan(const std::string& content, const Arg& pattern, const ScanContext& context);
    static ScanResult hybridScan(const std::string& content, const Arg& pattern, const ScanContext& context);
    
    // 辅助方法
    static void mergeResults(ScanResult& target, const ScanResult& source);
    static bool isValidScanInput(const std::string& content, const Arg& pattern);
    static void addError(ScanResult& result, const std::string& error);
    static void addWarning(ScanResult& result, const std::string& warning);
};

/**
 * CJMODScannerUtils - CJMOD扫描器工具类
 */
class CJMODScannerUtils {
public:
    /**
     * 预处理内容
     * @param content 原始内容
     * @return 预处理后的内容
     */
    static std::string preprocessContent(const std::string& content);
    
    /**
     * 后处理结果
     * @param result 原始结果
     * @return 后处理后的结果
     */
    static Arg postprocessResult(const Arg& result);
    
    /**
     * 验证扫描模式
     * @param pattern 模式
     * @return 是否有效
     */
    static bool validateScanPattern(const Arg& pattern);
    
    /**
     * 优化扫描结果
     * @param result 原始结果
     * @return 优化后的结果
     */
    static Arg optimizeResult(const Arg& result);
    
    /**
     * 格式化扫描结果
     * @param result 结果
     * @param format 格式化选项
     * @return 格式化后的字符串
     */
    static std::string formatResult(const Arg& result, const std::string& format = "default");
    
    /**
     * 比较扫描结果
     * @param a 结果a
     * @param b 结果b
     * @return 比较结果
     */
    static int compareResults(const Arg& a, const Arg& b);
    
    /**
     * 合并扫描结果
     * @param results 结果列表
     * @return 合并后的结果
     */
    static Arg mergeResults(const std::vector<Arg>& results);
    
    /**
     * 分离扫描结果
     * @param result 合并的结果
     * @param count 分离数量
     * @return 分离后的结果列表
     */
    static std::vector<Arg> separateResults(const Arg& result, size_t count);
};

} // namespace CJMOD
} // namespace CHTL