#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace CHTL {
namespace Scanner {

// 代码片段类型
enum class FragmentType {
    CHTL,           // CHTL代码片段
    CHTL_JS,        // CHTL JS代码片段
    CSS,            // CSS代码片段
    JAVASCRIPT,     // JavaScript代码片段
    HTML,           // HTML代码片段
    MIXED,          // 混合代码片段
    UNKNOWN
};

// 扫描策略
enum class ScanStrategy {
    NORMAL,         // 常规扫描
    COLLECT,        // 收集模式
    SKIP,           // 跳过模式
    DEEP_SCAN       // 深度扫描（用于复杂嵌套）
};

// 代码片段
struct CodeFragment {
    FragmentType type;
    std::string content;
    size_t start_position;
    size_t end_position;
    size_t line_number;
    size_t column_number;
    std::string file_path;
    std::vector<std::string> sub_fragments;  // 子片段（用于混合类型）
    
    CodeFragment() : type(FragmentType::UNKNOWN), start_position(0), end_position(0), 
                     line_number(0), column_number(0) {}
    
    CodeFragment(FragmentType t, const std::string& c, size_t start, size_t end)
        : type(t), content(c), start_position(start), end_position(end), 
          line_number(0), column_number(0) {}
    
    size_t getLength() const { return end_position - start_position; }
    bool isEmpty() const { return content.empty(); }
    bool isValid() const { return !content.empty() && end_position > start_position; }
    
    std::string toString() const;
};

// 扫描上下文
struct ScanContext {
    std::string current_file;
    size_t current_position;
    size_t current_line;
    size_t current_column;
    FragmentType expected_type;
    ScanStrategy strategy;
    std::unordered_map<std::string, std::string> context_vars;
    
    ScanContext() : current_position(0), current_line(1), current_column(1),
                    expected_type(FragmentType::CHTL), strategy(ScanStrategy::NORMAL) {}
};

// 扫描结果
struct ScanResult {
    std::vector<CodeFragment> fragments;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    bool success;
    
    ScanResult() : success(false) {}
    
    void addFragment(const CodeFragment& fragment) {
        fragments.push_back(fragment);
    }
    
    void addError(const std::string& error) {
        errors.push_back(error);
        success = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    size_t getFragmentCount() const { return fragments.size(); }
};

// CHTL统一扫描器
class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner();
    ~CHTLUnifiedScanner();
    
    // 主要扫描接口
    ScanResult scanSource(const std::string& source, const std::string& file_path = "");
    
    ScanResult scanFile(const std::string& file_path);
    
    std::vector<CodeFragment> scanToFragments(const std::string& source, 
                                              const std::string& file_path = "");
    
    // 特定类型扫描
    std::vector<CodeFragment> scanCHTLFragments(const std::string& source);
    
    std::vector<CodeFragment> scanCHTLJSFragments(const std::string& source);
    
    std::vector<CodeFragment> scanMixedFragments(const std::string& source);
    
    // 配置和设置
    void setChunkSize(size_t size) { chunk_size = size; }
    void setMaxFragmentSize(size_t size) { max_fragment_size = size; }
    void setDebugMode(bool debug) { debug_mode = debug; }
    void setStrictMode(bool strict) { strict_mode = strict; }
    
    // 策略配置
    void setScanStrategy(ScanStrategy strategy) { default_strategy = strategy; }
    void addCustomPattern(const std::string& pattern, FragmentType type);
    void removeCustomPattern(const std::string& pattern);
    
    // 错误处理
    std::vector<std::string> getErrors() const { return errors; }
    std::vector<std::string> getWarnings() const { return warnings; }
    void clearErrors() { errors.clear(); }
    void clearWarnings() { warnings.clear(); }
    
    // 性能统计
    size_t getProcessedBytes() const { return processed_bytes; }
    size_t getFragmentCount() const { return total_fragments; }
    double getLastScanTime() const { return last_scan_time; }
    
    // 重置状态
    void reset();
    
private:
    // 配置参数
    size_t chunk_size;              // 块大小
    size_t max_fragment_size;       // 最大片段大小
    bool debug_mode;                // 调试模式
    bool strict_mode;               // 严格模式
    ScanStrategy default_strategy;  // 默认策略
    
    // 状态变量
    std::string current_source;
    ScanContext context;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 性能统计
    size_t processed_bytes;
    size_t total_fragments;
    double last_scan_time;
    
    // 自定义模式
    std::unordered_map<std::string, FragmentType> custom_patterns;
    
    // 核心扫描方法
    std::vector<CodeFragment> performScan(const std::string& source, const ScanContext& ctx);
    
    // 可变长度切片扫描
    std::vector<CodeFragment> scanWithVariableSlicing(const std::string& source, 
                                                      const ScanContext& ctx);
    
    // 片段类型检测
    FragmentType detectFragmentType(const std::string& content, size_t position) const;
    
    FragmentType detectCHTLFragment(const std::string& content, size_t position) const;
    
    FragmentType detectCHTLJSFragment(const std::string& content, size_t position) const;
    
    FragmentType detectCSSFragment(const std::string& content, size_t position) const;
    
    FragmentType detectJSFragment(const std::string& content, size_t position) const;
    
    // 边界检测
    bool isValidCutPoint(const std::string& source, size_t position, FragmentType type) const;
    
    bool canFormCompleteFragment(const std::string& source, size_t start, size_t end) const;
    
    size_t findOptimalCutPoint(const std::string& source, size_t start, size_t max_end) const;
    
    // 最小单元切割
    std::vector<std::string> splitToMinimalUnits(const std::string& fragment, 
                                                  FragmentType type) const;
    
    std::vector<std::string> splitCHTLToUnits(const std::string& content) const;
    
    std::vector<std::string> splitCHTLJSToUnits(const std::string& content) const;
    
    // 上下文管理
    void updateContext(ScanContext& ctx, const std::string& content, size_t position) const;
    
    bool isInContext(const ScanContext& ctx, const std::string& context_name) const;
    
    // 模式匹配
    bool matchesPattern(const std::string& content, size_t position, 
                        const std::string& pattern) const;
    
    std::vector<size_t> findPatternPositions(const std::string& content, 
                                             const std::string& pattern) const;
    
    // CHTL特定检测
    bool isCHTLKeyword(const std::string& content, size_t position) const;
    
    bool isCHTLBlock(const std::string& content, size_t position) const;
    
    bool isCHTLComment(const std::string& content, size_t position) const;
    
    // CHTL JS特定检测
    bool isCHTLJSSelector(const std::string& content, size_t position) const;
    
    bool isCHTLJSArrow(const std::string& content, size_t position) const;
    
    bool isCHTLJSFunction(const std::string& content, size_t position) const;
    
    // 嵌套处理
    int calculateNestingLevel(const std::string& content, size_t position) const;
    
    std::pair<size_t, size_t> findMatchingBrace(const std::string& content, 
                                                size_t start_pos) const;
    
    // 边界优化
    size_t adjustStartBoundary(const std::string& source, size_t position, 
                               FragmentType type) const;
    
    size_t adjustEndBoundary(const std::string& source, size_t position, 
                             FragmentType type) const;
    
    // 验证和修正
    bool validateFragment(const CodeFragment& fragment) const;
    
    CodeFragment correctFragment(const CodeFragment& fragment) const;
    
    // 错误处理
    void addError(const std::string& message, size_t line = 0, size_t column = 0);
    
    void addWarning(const std::string& message, size_t line = 0, size_t column = 0);
    
    // 调试和日志
    void debugLog(const std::string& message) const;
    
    void dumpFragment(const CodeFragment& fragment) const;
    
    // 性能优化
    void optimizeFragments(std::vector<CodeFragment>& fragments) const;
    
    void mergeCompatibleFragments(std::vector<CodeFragment>& fragments) const;
    
    void eliminateEmptyFragments(std::vector<CodeFragment>& fragments) const;
    
    // 位置计算
    std::pair<size_t, size_t> calculateLineColumn(const std::string& source, 
                                                  size_t position) const;
    
    void updatePositionInfo(CodeFragment& fragment, const std::string& source) const;
};

// CJMOD扫描器支持类
class CJMODScanner {
public:
    CJMODScanner(CHTLUnifiedScanner* parent);
    ~CJMODScanner();
    
    // 双指针扫描
    std::vector<CodeFragment> doublePointerScan(const std::string& source, 
                                                const std::string& keyword);
    
    // 前置截取
    std::string prefixCapture(const std::string& source, size_t position, 
                              const std::string& keyword);
    
    // 关键字扫描
    void scanKeyword(const std::string& keyword, 
                     std::function<void()> callback);
    
    // 策略变更
    void policyChangeBegin(const std::string& delimiter, ScanStrategy policy);
    
    std::string policyChangeEnd(const std::string& delimiter, ScanStrategy policy);
    
    // 查看关键字
    std::string peekKeyword(int offset) const;
    
private:
    CHTLUnifiedScanner* parent_scanner;
    std::vector<std::string> keyword_buffer;
    size_t front_pointer;
    size_t back_pointer;
    ScanStrategy current_policy;
    std::string collected_content;
    
    // 内部方法
    bool isKeywordAt(const std::string& source, size_t position, 
                     const std::string& keyword) const;
    
    void movePointers(const std::string& source, const std::string& keyword);
    
    void collectContent(const std::string& source, size_t start, size_t end);
};

// 工具函数
class ScannerUtils {
public:
    // 获取片段类型名称
    static std::string getFragmentTypeName(FragmentType type);
    
    // 获取策略名称
    static std::string getStrategyName(ScanStrategy strategy);
    
    // 验证源代码
    static bool isValidSource(const std::string& source);
    
    // 估算扫描复杂度
    static size_t estimateScanComplexity(const std::string& source);
    
    // 优化建议
    static std::vector<std::string> getScanOptimizationSuggestions(const ScanResult& result);
    
    // 统计信息
    static std::unordered_map<std::string, size_t> getFragmentStatistics(
        const std::vector<CodeFragment>& fragments);
};

} // namespace Scanner
} // namespace CHTL