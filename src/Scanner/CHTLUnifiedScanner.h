#pragma once

#include "../Util/Common.h"

namespace CHTL {

// 代码片段类型
enum class FragmentType {
    CHTL,           // CHTL代码片段
    CHTL_JS,        // CHTL JS代码片段
    CSS,            // CSS代码片段
    JAVASCRIPT,     // 纯JavaScript代码片段
    HTML,           // HTML代码片段
    COMMENT,        // 注释片段
    WHITESPACE,     // 空白片段
    UNKNOWN         // 未知片段
};

// 代码片段
struct CodeFragment {
    FragmentType type;
    String content;
    size_t startPos;
    size_t endPos;
    size_t lineNumber;
    size_t columnNumber;
    
    CodeFragment() : type(FragmentType::UNKNOWN), startPos(0), endPos(0), lineNumber(1), columnNumber(1) {}
    CodeFragment(FragmentType t, const String& c, size_t start, size_t end, size_t line, size_t col)
        : type(t), content(c), startPos(start), endPos(end), lineNumber(line), columnNumber(col) {}
    
    bool isEmpty() const { return content.empty(); }
    size_t length() const { return content.length(); }
    String toString() const;
};

// 扫描器配置
struct ScannerConfig {
    bool enableVariableLengthSlicing = true;    // 启用可变长度切片
    bool enableContextValidation = true;        // 启用上下文验证
    bool enableMinimalUnitSplitting = true;     // 启用最小单元切割
    bool preserveWhitespace = false;            // 保留空白字符
    bool preserveComments = true;               // 保留注释
    size_t initialChunkSize = 1024;             // 初始块大小
    size_t maxChunkSize = 4096;                 // 最大块大小
    size_t minValidationLength = 16;            // 最小验证长度
};

// 统一扫描器
class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const String& source, const ScannerConfig& config = ScannerConfig{});
    ~CHTLUnifiedScanner() = default;
    
    // 主要扫描功能
    std::vector<CodeFragment> scanAll();
    CodeFragment scanNext();
    bool hasMoreFragments() const;
    void reset();
    
    // 配置管理
    void setConfig(const ScannerConfig& config) { this->config = config; }
    const ScannerConfig& getConfig() const { return config; }
    
    // 状态查询
    size_t getCurrentPosition() const { return currentPos; }
    size_t getTotalLength() const { return source.length(); }
    float getProgress() const { return static_cast<float>(currentPos) / source.length(); }
    
    // 错误处理
    bool hasErrors() const { return !errors.empty(); }
    StringVector getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
    // 统计信息
    struct ScanStats {
        size_t totalFragments = 0;
        size_t chtlFragments = 0;
        size_t chtljsFragments = 0;
        size_t cssFragments = 0;
        size_t jsFragments = 0;
        size_t commentFragments = 0;
        size_t whitespaceFragments = 0;
        size_t unknownFragments = 0;
        size_t totalChars = 0;
        double scanTime = 0.0;
    };
    
    ScanStats getStats() const { return stats; }
    
private:
    String source;
    size_t currentPos;
    size_t currentLine;
    size_t currentColumn;
    ScannerConfig config;
    StringVector errors;
    ScanStats stats;
    
    // 核心扫描逻辑
    CodeFragment scanFragment();
    FragmentType detectFragmentType(size_t start, size_t length);
    size_t findFragmentEnd(size_t start, FragmentType type);
    
    // 可变长度切片
    bool needsExtension(const String& fragment, FragmentType type);
    size_t extendFragment(size_t start, size_t currentEnd, FragmentType type);
    
    // 上下文验证
    bool isValidFragmentBoundary(size_t pos, FragmentType leftType, FragmentType rightType);
    bool validateCHTLFragment(const String& fragment);
    bool validateCHTLJSFragment(const String& fragment);
    
    // 最小单元切割
    std::vector<CodeFragment> splitToMinimalUnits(const CodeFragment& fragment);
    std::vector<size_t> findCHTLBreakpoints(const String& content);
    std::vector<size_t> findCHTLJSBreakpoints(const String& content);
    
    // 类型检测
    bool isCHTLKeyword(const String& word) const;
    bool isCHTLJSKeyword(const String& word) const;
    bool isCSSProperty(const String& word) const;
    bool isJSKeyword(const String& word) const;
    bool isHTMLTag(const String& word) const;
    
    // 模式匹配
    bool matchCHTLPattern(size_t pos) const;
    bool matchCHTLJSPattern(size_t pos) const;
    bool matchCSSPattern(size_t pos) const;
    bool matchJSPattern(size_t pos) const;
    bool matchCommentPattern(size_t pos) const;
    
    // 特殊语法检测
    bool isInStyleBlock(size_t pos) const;
    bool isInScriptBlock(size_t pos) const;
    bool isInTemplateBlock(size_t pos) const;
    bool isInStringLiteral(size_t pos) const;
    bool isInComment(size_t pos) const;
    
    // CHTL JS特殊检测
    bool isCHTLJSSelector(size_t pos) const;      // {{...}}
    bool isCHTLJSFunction(size_t pos) const;      // listen, animate等
    bool isCHTLJSEventBinding(size_t pos) const;  // &->
    bool isCHTLJSVirtualObject(size_t pos) const; // vir
    
    // 双指针扫描 (for CJMOD)
    struct PointerPair {
        size_t frontPtr;
        size_t backPtr;
        bool isCollecting;
    };
    
    CodeFragment dualPointerScan(const String& keyword);
    bool previewForKeyword(size_t pos, const String& keyword) const;
    void synchronizePointers(PointerPair& pointers);
    
    // 前置截取 (for CJMOD)
    String preemptiveCapture(size_t pos, const String& pattern);
    bool shouldPreempt(size_t pos, const String& pattern) const;
    void restoreFragment(size_t pos, const String& fragment);
    
    // 工具方法
    char charAt(size_t pos) const;
    String substring(size_t start, size_t length) const;
    bool isAtEnd(size_t pos) const;
    void skipWhitespace(size_t& pos) const;
    void skipToNextLine(size_t& pos) const;
    
    // 位置管理
    void updatePosition(char c);
    void resetPosition();
    
    // 错误报告
    void reportError(const String& message, size_t pos = SIZE_MAX);
    void reportInvalidFragment(FragmentType type, size_t pos);
    void reportIncompleteFragment(size_t pos);
    
    // 统计更新
    void updateStats(const CodeFragment& fragment);
    void incrementFragmentCount(FragmentType type);
};

// 扫描器工厂
class ScannerFactory {
public:
    static std::unique_ptr<CHTLUnifiedScanner> createScanner(const String& source);
    static std::unique_ptr<CHTLUnifiedScanner> createScannerWithConfig(
        const String& source, const ScannerConfig& config);
    static ScannerConfig getDefaultConfig();
    static ScannerConfig getPerformanceConfig();
    static ScannerConfig getDebugConfig();
};

} // namespace CHTL