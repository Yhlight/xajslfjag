#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace chtl {

// 代码片段类型
enum class CodeFragmentType {
    CHTL,       // CHTL代码片段
    CHTL_JS,    // CHTL JS代码片段
    CSS,        // CSS代码片段
    JS,         // JavaScript代码片段
    HTML,       // 原始HTML片段
    TEXT,       // 文本内容
    UNKNOWN     // 未知类型
};

// 扫描器政策
enum class ScannerPolicy {
    NORMAL,     // 常规扫描
    COLLECT,    // 收集模式（用于处理复杂参数值）
    SKIP        // 跳过模式
};

// 代码片段结构
struct CodeFragment {
    CodeFragmentType type;
    std::string content;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    std::string filePath;
    
    CodeFragment() : type(CodeFragmentType::UNKNOWN), 
                     startLine(0), startColumn(0), 
                     endLine(0), endColumn(0) {}
};

// 扫描器配置
struct ScannerConfig {
    size_t initialSliceSize = 1024;      // 初始切片大小
    size_t maxSliceSize = 65536;         // 最大切片大小
    bool enableVariableSlicing = true;   // 启用可变长度切片
    bool enableContextualAnalysis = true; // 启用上下文分析
};

class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner();
    explicit CHTLUnifiedScanner(const ScannerConfig& config);
    ~CHTLUnifiedScanner();
    
    // 主要扫描接口
    std::vector<CodeFragment> scan(const std::string& code, const std::string& filePath = "");
    std::vector<CodeFragment> scanFile(const std::string& filePath);
    
    // 政策管理
    void setPolicyChangeBegin(const std::string& delimiter, ScannerPolicy policy);
    std::string setPolicyChangeEnd(const std::string& delimiter, ScannerPolicy policy);
    ScannerPolicy getCurrentPolicy() const { return currentPolicy; }
    
    // CJMOD特殊扫描器接口
    class CJMODScanner {
    public:
        CJMODScanner(CHTLUnifiedScanner* parent) : parent(parent) {}
        
        // 扫描关键字
        void scanKeyword(const std::string& keyword, std::function<void()> callback);
        
        // 双指针扫描
        std::string scanWithDoublePointer(const std::string& code, 
                                         const std::string& startKeyword, 
                                         const std::string& endKeyword);
        
        // 前置截取
        std::string preCut(const std::string& code, const std::string& keyword);
        
        // 辅助函数
        std::string peekKeyword(int offset) const;
        
    private:
        CHTLUnifiedScanner* parent;
        size_t currentPosition = 0;
        std::vector<std::string> capturedKeywords;
    };
    
    // 获取CJMOD扫描器
    std::unique_ptr<CJMODScanner> getCJMODScanner();
    
private:
    // 内部扫描方法
    bool isCHTLKeyword(const std::string& token) const;
    bool isCHTLJSKeyword(const std::string& token) const;
    bool isValidSlicePoint(const std::string& code, size_t position) const;
    
    // 切片管理
    std::vector<std::string> createVariableSlices(const std::string& code);
    void adjustSliceBoundary(std::string& slice, const std::string& nextSlice);
    
    // 片段分析
    CodeFragmentType analyzeFragment(const std::string& fragment) const;
    bool isCompleteFragment(const std::string& fragment, CodeFragmentType type) const;
    
    // 二次切割
    std::vector<CodeFragment> secondarySplit(const std::string& fragment, 
                                            CodeFragmentType type,
                                            size_t startLine,
                                            size_t startColumn);
    
    // 上下文分析
    void analyzeContext(std::vector<CodeFragment>& fragments);
    bool shouldMergeFragments(const CodeFragment& prev, const CodeFragment& current) const;
    
    // 辅助方法
    std::pair<size_t, size_t> calculatePosition(const std::string& code, size_t offset) const;
    
private:
    ScannerConfig config;
    ScannerPolicy currentPolicy;
    std::string policyDelimiterStart;
    std::string policyDelimiterEnd;
    std::string collectedContent;
    
    // CHTL关键字集合
    static const std::unordered_map<std::string, bool> chtlKeywords;
    static const std::unordered_map<std::string, bool> chtljsKeywords;
};

} // namespace chtl

#endif // CHTL_UNIFIED_SCANNER_H