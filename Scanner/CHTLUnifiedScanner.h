#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace Scanner {

// 代码片段类型
enum class CodeFragmentType {
    CHTL,           // CHTL代码片段
    CHTL_JS,        // CHTL JS代码片段
    CSS,            // CSS代码片段
    JAVASCRIPT,     // JavaScript代码片段
    UNKNOWN         // 未知类型
};

// 代码片段信息
struct CodeFragment {
    CodeFragmentType type;
    std::string content;
    size_t startPos;
    size_t endPos;
    size_t line;
    size_t column;
    
    CodeFragment(CodeFragmentType type, const std::string& content, 
                 size_t startPos, size_t endPos, size_t line, size_t column)
        : type(type), content(content), startPos(startPos), endPos(endPos), 
          line(line), column(column) {}
};

// 扫描器状态
enum class ScannerState {
    INITIAL,        // 初始状态
    SCANNING_CHTL, // 扫描CHTL
    SCANNING_CHTL_JS, // 扫描CHTL JS
    SCANNING_CSS,  // 扫描CSS
    SCANNING_JS,   // 扫描JavaScript
    ERROR           // 错误状态
};

// CHTL统一扫描器
class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner();
    ~CHTLUnifiedScanner() = default;
    
    // 设置源代码
    void setSource(const std::string& source);
    
    // 执行扫描
    std::vector<CodeFragment> scan();
    
    // 获取扫描结果
    const std::vector<CodeFragment>& getFragments() const { return fragments_; }
    
    // 重置扫描器
    void reset();
    
    // 获取当前状态
    ScannerState getCurrentState() const { return currentState_; }
    
    // 获取源代码
    const std::string& getSource() const { return source_; }
    
private:
    std::string source_;
    std::vector<CodeFragment> fragments_;
    ScannerState currentState_;
    size_t currentPos_;
    size_t currentLine_;
    size_t currentColumn_;
    
    // 扫描配置
    struct ScanConfig {
        size_t minFragmentSize;      // 最小片段大小
        size_t maxFragmentSize;      // 最大片段大小
        size_t lookAheadSize;        // 前瞻大小
        bool enableSmartCutting;     // 启用智能切割
    } config_;
    
    // 辅助函数
    void initializeConfig();
    void scanNextFragment();
    CodeFragmentType detectFragmentType(size_t startPos, size_t endPos);
    CodeFragmentType detectCHTLType(size_t startPos, size_t endPos);
    CodeFragmentType detectCHTLJSType(size_t startPos, size_t endPos);
    CodeFragmentType detectCSSType(size_t startPos, size_t endPos);
    CodeFragmentType detectJSType(size_t startPos, size_t endPos);
    
    // 智能切割
    size_t findOptimalCutPoint(size_t startPos, size_t maxSize);
    bool isValidCutPoint(size_t pos);
    bool isCompleteExpression(size_t startPos, size_t endPos);
    
    // 上下文分析
    bool hasUnclosedBrackets(size_t startPos, size_t endPos);
    bool hasUnclosedBraces(size_t startPos, size_t endPos);
    bool hasUnclosedParens(size_t startPos, size_t endPos);
    bool hasUnclosedQuotes(size_t startPos, size_t endPos);
    
    // 字符处理
    char peek(size_t offset = 0) const;
    void advance(size_t count = 1);
    void advanceLine();
    bool isEOF() const;
    
    // 状态管理
    void setState(ScannerState state);
    void logStateTransition(ScannerState from, ScannerState to);
};

} // namespace Scanner