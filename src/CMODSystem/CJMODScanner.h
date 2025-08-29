#pragma once

#include "../Util/Common.h"
#include "../CHTL/CHTLLexer/GlobalMap.h"
#include "Syntax.h"
#include <vector>
#include <unordered_map>

namespace CJMOD {

// 前置声明
class Arg;

// 扫描器状态
struct ScannerState {
    size_t position;
    size_t line;
    size_t column;
    bool isValid;
    
    ScannerState();
    void advance(char c);
    CHTL::Position getCurrentPosition() const;
    bool isAtEnd(const CHTL::String& source) const;
    void reset() { position = 0; line = 1; column = 1; isValid = true; }
};

// 双指针扫描结果
struct DualPointerScanResult {
    CHTL::StringVector fragments;
    CHTL::StringVector keywords;
    CHTL::StringVector errors;
    CHTL::StringVector warnings;
    std::unordered_map<CHTL::String, CHTL::String> metadata;
    bool success = false;
    
    bool hasErrors() const;
    bool hasWarnings() const;
    CHTL::String getErrorSummary() const;
    CHTL::String getWarningSummary() const;
};

// CJMOD扫描器配置
struct CJMODScannerConfig {
    bool enableDualPointer = true;
    bool enablePreemptiveCapture = true;
    bool enableKeywordValidation = true;
    bool enableFragmentValidation = true;
    bool enablePostProcessing = true;
    bool strictMode = false;
    size_t maxScanDepth = 1000;
    CHTL::StringVector customKeywords;
    CHTL::StringVector ignorePatterns;
};

// CJMOD扫描器主类
class CJMODScanner {
public:
    // 构造函数
    CJMODScanner(const CHTL::String& source, const CJMODScannerConfig& config = CJMODScannerConfig{});
    
    // 实例扫描方法
    DualPointerScanResult scan();
    
    // 关键的静态扫描方法 - 动态获取值的核心
    static Arg scan(const Arg& args, const CHTL::String& keyword);
    
    // 前置截取机制
    static CHTL::String preemptiveCapture(const CHTL::String& source, const CHTL::String& pattern);
    
    // 语法片段验证
    static bool validateSyntaxFragment(const CHTL::String& fragment);
    
    // 工具方法
    void resetScanners();
    const CHTL::StringVector& getErrors() const { return errors; }
    const CHTL::StringVector& getWarnings() const { return warnings; }
    
private:
    // 成员变量
    CHTL::String sourceCode;
    CJMODScannerConfig config;
    ScannerState primaryState;
    ScannerState auxiliaryState;
    CHTL::StringVector errors;
    CHTL::StringVector warnings;
    
    // 核心算法实现
    void performDualPointerScan(DualPointerScanResult& result);
    void scanWithPrimaryPointer(DualPointerScanResult& result);
    void scanWithAuxiliaryPointer(DualPointerScanResult& result);
    
    // 关键字检测
    bool detectKeyword(const CHTL::String& keyword, size_t position);
    void processKeywordFound(const CHTL::String& keyword, DualPointerScanResult& result);
    
    // 片段处理
    void collectFragment(size_t start, size_t end, DualPointerScanResult& result);
    void validateFragment(const CHTL::String& fragment, DualPointerScanResult& result);
    
    // 后处理
    void postProcessResults(DualPointerScanResult& result);
    void validateScanResults(DualPointerScanResult& result);
    
    // 初始化
    void initializePattern();
    
    // 静态辅助方法
    static CHTL::StringVector extractArgumentsFromSource(const CHTL::String& source, const CHTL::String& keyword);
    static bool isValidCJMODKeyword(const CHTL::String& keyword);
    static CHTL::String cleanupFragment(const CHTL::String& fragment);
};

} // namespace CJMOD