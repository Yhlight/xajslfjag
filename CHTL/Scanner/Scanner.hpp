#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

/**
 * 代码片段类型枚举
 */
enum class FragmentType {
    CHTL,                 // CHTL代码片段
    CHTL_JS,              // CHTL JS代码片段
    CSS,                  // CSS代码片段
    JAVASCRIPT,           // JavaScript代码片段
    HTML,                 // HTML代码片段
    UNKNOWN               // 未知类型
};

/**
 * 代码片段结构
 */
struct CodeFragment {
    FragmentType type;
    std::string content;
    size_t startPosition;
    size_t endPosition;
    size_t line;
    size_t column;
    bool isComplete;      // 是否是完整的片段
    bool needsProcessing; // 是否需要进一步处理
    
    CodeFragment() : type(FragmentType::UNKNOWN), startPosition(0), endPosition(0), 
                    line(0), column(0), isComplete(false), needsProcessing(false) {}
    
    CodeFragment(FragmentType t, const std::string& c, size_t start, size_t end)
        : type(t), content(c), startPosition(start), endPosition(end), 
          line(0), column(0), isComplete(true), needsProcessing(false) {}
    
    size_t getLength() const { return endPosition - startPosition; }
    bool isEmpty() const { return content.empty(); }
};

/**
 * CHTL统一扫描器
 * 负责将混合的CHTL代码精准切割成不同类型的片段
 * 支持可变长度切片和上下文感知切割
 */
class CHTLUnifiedScanner {
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    std::vector<CodeFragment> fragments;
    
    // 扫描状态
    FragmentType currentFragmentType;
    size_t fragmentStart;
    int braceDepth;
    int parenDepth;
    int bracketDepth;
    bool inString;
    bool inComment;
    char stringDelimiter;
    
    // 关键字检测
    std::vector<std::string> chtlKeywords;
    std::vector<std::string> chtlJSKeywords;
    
    // 扫描方法
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    
    // 片段检测
    FragmentType detectFragmentType(size_t startPos) const;
    bool isCHTLKeyword(const std::string& word) const;
    bool isCHTLJSKeyword(const std::string& word) const;
    bool isHTMLTag(const std::string& tag) const;
    
    // 边界检测
    bool isFragmentBoundary() const;
    bool isValidCutPoint() const;
    bool needsForwardExpansion(size_t endPos) const;
    
    // 特殊语法检测
    bool detectCHTLJSSelector() const;          // 检测{{selector}}
    bool detectCHTLReference() const;           // 检测&引用
    bool detectTemplateVariable() const;        // 检测模板变量引用
    
    // 切割优化
    void optimizeCut(size_t& startPos, size_t& endPos);
    void ensureMinimalUnit(size_t& startPos, size_t& endPos, FragmentType type);
    void avoidFragmentation(size_t& startPos, size_t& endPos);
    
    // 完整性检查
    bool isCompleteFragment(const std::string& fragment, FragmentType type) const;
    bool needsExpansion(const std::string& fragment, FragmentType type) const;
    void expandFragment(size_t& endPos, FragmentType type);
    
    // 上下文分析
    FragmentType analyzeContext(size_t pos) const;
    bool isInStyleBlock(size_t pos) const;
    bool isInScriptBlock(size_t pos) const;
    bool isInTemplateBlock(size_t pos) const;
    
    // 二次切割
    std::vector<CodeFragment> secondaryCut(const CodeFragment& fragment);
    std::vector<CodeFragment> cutCHTLFragment(const CodeFragment& fragment);
    std::vector<CodeFragment> cutCHTLJSFragment(const CodeFragment& fragment);

public:
    CHTLUnifiedScanner();
    explicit CHTLUnifiedScanner(const std::string& source);
    
    // 主要接口
    void setSource(const std::string& source);
    std::vector<CodeFragment> scan();
    const std::vector<CodeFragment>& getFragments() const;
    
    // 配置
    void setMinFragmentSize(size_t size);
    void setMaxFragmentSize(size_t size);
    void enableContextAnalysis(bool enable);
    void enableOptimization(bool enable);
    
    // 工具方法
    void reset();
    size_t getCurrentPosition() const;
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    
    // 调试
    void dumpFragments() const;
    std::string getFragmentsAsString() const;
    
    // 验证
    bool validateFragments() const;
    std::vector<std::string> getValidationErrors() const;

private:
    size_t minFragmentSize;
    size_t maxFragmentSize;
    bool contextAnalysisEnabled;
    bool optimizationEnabled;
    
    void initializeKeywords();
    void reportError(const std::string& message) const;
};

/**
 * 双指针扫描器
 * 用于CJMOD的特殊扫描需求
 */
class DualPointerScanner {
private:
    std::string source;
    size_t frontPointer;
    size_t backPointer;
    std::vector<std::string> keywords;
    bool collectingMode;

public:
    DualPointerScanner(const std::string& source, const std::vector<std::string>& keywords);
    
    // 扫描方法
    void scan();
    void moveToNextKeyword();
    void startCollection();
    void stopCollection();
    
    // 结果获取
    std::string getCollectedFragment() const;
    bool hasKeywordAtFront() const;
    
    // 指针控制
    void resetPointers();
    void syncPointers();
    size_t getFrontPosition() const;
    size_t getBackPosition() const;
};

/**
 * 前置截取扫描器
 * 用于处理特殊语法如 arg ** arg2
 */
class PrefixCutScanner {
private:
    std::string source;
    std::vector<std::string> specialOperators;
    
public:
    PrefixCutScanner(const std::string& source);
    
    // 配置
    void addSpecialOperator(const std::string& op);
    void removeSpecialOperator(const std::string& op);
    
    // 扫描
    std::vector<CodeFragment> scanWithPrefixCut();
    bool needsPrefixCut(size_t position) const;
    size_t findCutPosition(size_t operatorPos) const;
    
    // 截取
    std::string cutPrefix(size_t position, size_t length);
    void restorePrefix(size_t position, const std::string& prefix);
};

/**
 * 扫描器工厂
 * 提供不同配置的扫描器实例
 */
class ScannerFactory {
public:
    static std::unique_ptr<CHTLUnifiedScanner> createDefaultScanner();
    static std::unique_ptr<CHTLUnifiedScanner> createOptimizedScanner();
    static std::unique_ptr<CHTLUnifiedScanner> createDebugScanner();
    static std::unique_ptr<DualPointerScanner> createCJMODScanner(
        const std::string& source, const std::vector<std::string>& keywords);
    static std::unique_ptr<PrefixCutScanner> createPrefixScanner(const std::string& source);
};

}