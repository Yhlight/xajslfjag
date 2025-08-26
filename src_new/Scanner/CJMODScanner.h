#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>

namespace CHTL {
namespace Scanner {

// CJMOD扫描类型
enum class CJMODScanType {
    DUAL_POINTER_SCAN,      // 双指针扫描
    PREFIX_CAPTURE,         // 前置截取
    SLIDING_WINDOW,         // 滑动窗口
    KEYWORD_DETECTION,      // 关键字检测
    FRAGMENT_COLLECTION     // 片段收集
};

// 扫描指针状态
struct ScanPointer {
    size_t position;                // 当前位置
    size_t start_position;          // 开始位置
    size_t end_position;            // 结束位置
    bool is_active;                 // 是否激活
    bool is_collecting;             // 是否正在收集
    std::string collected_content;  // 收集的内容
    
    ScanPointer() : position(0), start_position(0), end_position(0), is_active(false), is_collecting(false) {}
    
    void reset() {
        position = 0;
        start_position = 0;
        end_position = 0;
        is_active = false;
        is_collecting = false;
        collected_content.clear();
    }
    
    void startCollection(size_t start_pos) {
        start_position = start_pos;
        is_collecting = true;
        collected_content.clear();
    }
    
    void endCollection(size_t end_pos) {
        end_position = end_pos;
        is_collecting = false;
    }
    
    size_t length() const {
        return end_position > start_position ? end_position - start_position : 0;
    }
};

// CJMOD语法片段
struct CJMODFragment {
    std::string keyword;            // 关键字
    std::string prefix_content;     // 前置内容
    std::string main_content;       // 主要内容
    std::string suffix_content;     // 后置内容
    size_t start_position;          // 起始位置
    size_t end_position;            // 结束位置
    CJMODScanType scan_type;        // 扫描类型
    bool is_complete;               // 是否完整
    std::unordered_map<std::string, std::string> parameters;  // 参数
    
    CJMODFragment() : start_position(0), end_position(0), 
                     scan_type(CJMODScanType::DUAL_POINTER_SCAN), is_complete(false) {}
    
    std::string getFullContent() const {
        return prefix_content + keyword + main_content + suffix_content;
    }
    
    size_t length() const {
        return end_position > start_position ? end_position - start_position : 0;
    }
    
    bool isEmpty() const {
        return keyword.empty() && main_content.empty();
    }
};

// 扫描结果
struct CJMODScanResult {
    bool success;                           // 扫描是否成功
    std::vector<CJMODFragment> fragments;   // 扫描到的片段
    std::vector<std::string> errors;        // 错误信息
    std::vector<std::string> warnings;      // 警告信息
    size_t total_processed;                 // 总处理字符数
    size_t fragments_found;                 // 找到的片段数
    
    CJMODScanResult() : success(false), total_processed(0), fragments_found(0) {}
    
    void addFragment(const CJMODFragment& fragment) {
        fragments.push_back(fragment);
        fragments_found++;
    }
    
    void addError(const std::string& error) {
        errors.push_back(error);
        success = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// CJMOD扫描器
class CJMODScanner {
private:
    // 双指针系统
    ScanPointer front_pointer;      // 前指针
    ScanPointer back_pointer;       // 后指针
    
    // 扫描配置
    std::unordered_set<std::string> cjmod_keywords;     // CJMOD关键字集合
    std::unordered_set<std::string> prefix_keywords;    // 前置关键字
    std::unordered_map<std::string, std::string> keyword_patterns;  // 关键字模式
    
    // 扫描状态
    bool dual_pointer_active;       // 双指针是否激活
    bool prefix_capture_active;     // 前置截取是否激活
    size_t scan_window_size;        // 扫描窗口大小
    size_t max_fragment_size;       // 最大片段大小
    
    // 缓存和优化
    std::string current_content;    // 当前扫描内容
    std::unordered_map<std::string, CJMODFragment> fragment_cache;  // 片段缓存
    
    // 内部方法
    void initializeCJMODKeywords();
    void initializePrefixKeywords();
    void initializeKeywordPatterns();
    
    bool detectKeywordAtPosition(const std::string& content, size_t position, std::string& detected_keyword);
    bool needsPrefixCapture(const std::string& keyword, const std::string& preceding_content);
    size_t findKeywordEnd(const std::string& content, size_t start_position, const std::string& keyword);
    std::string extractPrefixContent(const std::string& content, size_t keyword_position);
    std::string extractMainContent(const std::string& content, size_t start_position, size_t end_position);
    
    // 双指针扫描实现
    CJMODScanResult performDualPointerScan(const std::string& content);
    void advanceFrontPointer(const std::string& content);
    void advanceBackPointer(const std::string& content);
    void synchronizePointers();
    
    // 前置截取实现
    CJMODScanResult performPrefixCapture(const std::string& content);
    std::string capturePrefix(const std::string& content, size_t keyword_position);
    void adjustContentAfterCapture(std::string& content, const std::string& captured_prefix);
    
    // 滑动窗口实现
    CJMODScanResult performSlidingWindowScan(const std::string& content);
    bool processWindow(const std::string& window_content, size_t window_start);
    
public:
    CJMODScanner();
    ~CJMODScanner() = default;
    
    // 主要扫描接口
    CJMODScanResult scanContent(const std::string& content, CJMODScanType scan_type = CJMODScanType::DUAL_POINTER_SCAN);
    CJMODScanResult scanFragment(const std::string& fragment);
    std::vector<CJMODFragment> extractCJMODFragments(const std::string& content);
    
    // 双指针扫描
    CJMODScanResult dualPointerScan(const std::string& content);
    void resetPointers();
    void configureDualPointerScan(size_t window_size = 256);
    
    // 前置截取
    CJMODScanResult prefixCaptureScan(const std::string& content);
    std::string performPrefixCapture(const std::string& content, const std::string& keyword);
    void configurePrefixCapture(size_t max_prefix_length = 128);
    
    // 关键字管理
    void addCJMODKeyword(const std::string& keyword);
    void removeCJMODKeyword(const std::string& keyword);
    bool isCJMODKeyword(const std::string& keyword) const;
    std::vector<std::string> getAllCJMODKeywords() const;
    
    // 模式管理
    void addKeywordPattern(const std::string& keyword, const std::string& pattern);
    void removeKeywordPattern(const std::string& keyword);
    std::string getKeywordPattern(const std::string& keyword) const;
    
    // 配置管理
    void setWindowSize(size_t size);
    void setMaxFragmentSize(size_t size);
    void enableDualPointerMode(bool enable = true);
    void enablePrefixCaptureMode(bool enable = true);
    
    // 片段验证
    bool validateCJMODFragment(const CJMODFragment& fragment);
    bool isCompleteFragment(const CJMODFragment& fragment);
    std::vector<std::string> getFragmentErrors(const CJMODFragment& fragment);
    
    // 性能优化
    void enableFragmentCache(bool enable = true);
    void clearFragmentCache();
    void preloadKeywords(const std::vector<std::string>& keywords);
    
    // 调试和分析
    std::string generateScanReport(const CJMODScanResult& result) const;
    void dumpScannerState() const;
    void dumpPointerStates() const;
    
    // 错误处理
    std::vector<std::string> getLastErrors() const;
    std::vector<std::string> getLastWarnings() const;
    void clearErrors();
    void clearWarnings();
    
private:
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    void addError(const std::string& message);
    void addWarning(const std::string& message);
    
    // 内部工具方法
    bool isWhitespace(char c) const;
    bool isKeywordChar(char c) const;
    size_t skipWhitespace(const std::string& content, size_t position) const;
    size_t findNextNonWhitespace(const std::string& content, size_t position) const;
    
    // 内部验证方法
    bool isValidKeywordPosition(const std::string& content, size_t position) const;
    bool isValidFragmentBoundary(const std::string& content, size_t start, size_t end) const;
    
    // 性能统计
    mutable size_t scan_count;
    mutable size_t cache_hits;
    mutable size_t cache_misses;
    
    void updatePerformanceStats(bool cache_hit) const;
    void resetPerformanceStats();
};

// CJMOD扫描工具类
class CJMODScannerUtils {
public:
    // 关键字检测
    static bool containsCJMODKeywords(const std::string& content);
    static std::vector<std::string> extractKeywords(const std::string& content);
    static std::vector<size_t> findKeywordPositions(const std::string& content, const std::string& keyword);
    
    // 内容分析
    static bool needsPrefixCapture(const std::string& content, const std::string& keyword);
    static bool hasCJMODSyntax(const std::string& content);
    static std::string normalizeCJMODContent(const std::string& content);
    
    // 片段处理
    static std::vector<std::string> splitIntoFragments(const std::string& content);
    static std::string mergeFragments(const std::vector<CJMODFragment>& fragments);
    static bool isValidCJMODFragment(const std::string& fragment);
    
    // 扫描策略
    static CJMODScanType determineBestScanType(const std::string& content);
    static size_t calculateOptimalWindowSize(const std::string& content);
    static bool shouldUseDualPointer(const std::string& content);
    static bool shouldUsePrefixCapture(const std::string& content);
    
    // 性能优化
    static std::string optimizeContentForScanning(const std::string& content);
    static std::vector<std::string> preprocessContent(const std::string& content);
    static bool isEfficientScanCandidate(const std::string& content);
    
    // 验证和检查
    static bool validateScanResult(const CJMODScanResult& result);
    static std::vector<std::string> checkFragmentIntegrity(const std::vector<CJMODFragment>& fragments);
    static bool hasOverlappingFragments(const std::vector<CJMODFragment>& fragments);
};

// 预定义CJMOD关键字
namespace PredefinedCJMODKeywords {
    extern const std::vector<std::string> STANDARD_KEYWORDS;
    extern const std::vector<std::string> CHTHOLLY_KEYWORDS;
    extern const std::vector<std::string> FUNCTION_KEYWORDS;
    extern const std::vector<std::string> OPERATOR_KEYWORDS;
}

} // namespace Scanner
} // namespace CHTL