#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

// 代码片段类型
enum class CodeFragmentType {
    CHTL,           // CHTL语法片段
    CHTL_JS,        // CHTL JS语法片段
    CSS,            // CSS语法片段
    JS,             // JavaScript语法片段
    UNKNOWN
};

// 代码片段
struct CodeFragment {
    CodeFragmentType type;
    std::string content;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    size_t startPosition;
    size_t endPosition;
    
    CodeFragment(CodeFragmentType t = CodeFragmentType::UNKNOWN,
                const std::string& c = "",
                size_t sl = 0, size_t sc = 0,
                size_t el = 0, size_t ec = 0,
                size_t sp = 0, size_t ep = 0)
        : type(t), content(c), startLine(sl), startColumn(sc),
          endLine(el), endColumn(ec), startPosition(sp), endPosition(ep) {}
};

using CodeFragmentPtr = std::shared_ptr<CodeFragment>;

// CHTL统一扫描器 - 精准代码切割器
class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner();
    
    // 设置输入源码
    void SetInput(const std::string& input);
    
    // 扫描并切割代码
    std::vector<CodeFragmentPtr> ScanAndSlice();
    
    // 配置扫描器
    void SetSliceSize(size_t size);               // 设置初始切片大小
    void SetMinFragmentSize(size_t size);         // 设置最小片段大小
    void SetMaxFragmentSize(size_t size);         // 设置最大片段大小
    void EnableDebugMode(bool enable = true);     // 启用调试模式
    
    // 错误处理
    bool HasErrors() const;
    const std::vector<std::string>& GetErrors() const;
    void ClearErrors();
    
    // 状态查询
    size_t GetTotalFragments() const;
    void PrintFragments(const std::vector<CodeFragmentPtr>& fragments) const;
    
private:
    std::string input_;
    size_t sliceSize_;
    size_t minFragmentSize_;
    size_t maxFragmentSize_;
    bool debugMode_;
    std::vector<std::string> errors_;
    
    // 内部状态
    size_t position_;
    size_t line_;
    size_t column_;
    
    // 核心扫描方法
    std::vector<CodeFragmentPtr> PerformSlicing();
    CodeFragmentPtr AnalyzeSlice(size_t start, size_t end);
    
    // 片段类型识别
    CodeFragmentType DetermineFragmentType(const std::string& content, size_t start, size_t end);
    bool IsCHTLFragment(const std::string& content) const;
    bool IsCHTLJSFragment(const std::string& content) const;
    bool IsCSSFragment(const std::string& content) const;
    bool IsJSFragment(const std::string& content) const;
    
    // 边界调整 - 确保片段完整性
    size_t AdjustFragmentBoundary(size_t position, CodeFragmentType expectedType);
    bool IsValidCHTLBoundary(size_t position) const;
    bool IsValidCHTLJSBoundary(size_t position) const;
    bool IsValidCSSBoundary(size_t position) const;
    bool IsValidJSBoundary(size_t position) const;
    
    // 最小单元切割
    std::vector<CodeFragmentPtr> PerformMinimalUnitSlicing(const CodeFragmentPtr& fragment);
    std::vector<size_t> FindCHTLMinimalUnits(const std::string& content) const;
    std::vector<size_t> FindCHTLJSMinimalUnits(const std::string& content) const;
    
    // 上下文分析
    bool RequiresContextMerging(const CodeFragmentPtr& current, const CodeFragmentPtr& next) const;
    CodeFragmentPtr MergeFragments(const CodeFragmentPtr& first, const CodeFragmentPtr& second) const;
    
    // 验证片段完整性
    bool ValidateFragmentIntegrity(const CodeFragmentPtr& fragment) const;
    bool IsCHTLComplete(const std::string& content) const;
    bool IsCHTLJSComplete(const std::string& content) const;
    
    // 辅助方法
    char GetCharAt(size_t position) const;
    std::string GetSubstring(size_t start, size_t end) const;
    bool MatchPattern(size_t position, const std::string& pattern) const;
    void UpdatePosition(size_t newPosition);
    void ReportError(const std::string& message);
    
    // 特殊处理方法
    size_t HandleCHTLSelector(size_t position) const;       // 处理{{selector}}
    size_t HandleCHTLJSFunction(size_t position) const;     // 处理CHTL JS函数
    size_t HandleStringLiteral(size_t position, char delimiter) const;
    size_t HandleComment(size_t position) const;
    
    // 优化方法
    void OptimizeFragmentSequence(std::vector<CodeFragmentPtr>& fragments);
    void MergeAdjacentSameTypeFragments(std::vector<CodeFragmentPtr>& fragments);
    void AdjustFragmentSizes(std::vector<CodeFragmentPtr>& fragments);
};

} // namespace CHTL