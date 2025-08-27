#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace CHTL {

/// 代码片段类型枚举
enum class FragmentType {
    CHTL,           // CHTL代码片段
    CHTL_JS,        // CHTL JS代码片段
    CSS,            // CSS代码片段
    JS,             // JavaScript代码片段
    MIXED,          // 混合片段（需要进一步分析）
    UNKNOWN         // 未知类型
};

/// 代码片段结构
struct CodeFragment {
    std::string content;        // 片段内容
    FragmentType type;          // 片段类型
    size_t startLine;          // 起始行号
    size_t startColumn;        // 起始列号
    size_t endLine;            // 结束行号
    size_t endColumn;          // 结束列号
    size_t startOffset;        // 起始偏移量
    size_t endOffset;          // 结束偏移量
    
    CodeFragment(const std::string& c, FragmentType t, 
                size_t sl, size_t sc, size_t el, size_t ec,
                size_t so, size_t eo)
        : content(c), type(t), startLine(sl), startColumn(sc)
        , endLine(el), endColumn(ec), startOffset(so), endOffset(eo) {}
};

/// 扫描配置结构
struct ScannerConfig {
    size_t initialChunkSize;        // 初始切片大小
    size_t maxChunkSize;           // 最大切片大小
    size_t minChunkSize;           // 最小切片大小
    bool enableContextAnalysis;     // 启用上下文分析
    bool enableMinimalUnitSplit;    // 启用最小单元切割
    bool enableAdaptiveExpansion;   // 启用自适应扩展
    
    ScannerConfig()
        : initialChunkSize(1024)
        , maxChunkSize(8192)
        , minChunkSize(64)
        , enableContextAnalysis(true)
        , enableMinimalUnitSplit(true)
        , enableAdaptiveExpansion(true) {}
};

/// CHTL统一扫描器类
/// 负责精准的代码切割，支持可变长度切片
class CHTLUnifiedScanner {
private:
    /// 源代码
    std::string sourceCode;
    
    /// 扫描配置
    ScannerConfig config;
    
    /// 代码片段列表
    std::vector<CodeFragment> fragments;
    
    /// 当前扫描位置
    size_t currentPosition;
    
    /// 当前行号
    size_t currentLine;
    
    /// 当前列号
    size_t currentColumn;
    
    /// 错误信息列表
    std::vector<std::string> errors;
    
    /// 警告信息列表
    std::vector<std::string> warnings;

public:
    /// 构造函数
    explicit CHTLUnifiedScanner(const ScannerConfig& cfg = ScannerConfig());
    
    /// 析构函数
    ~CHTLUnifiedScanner();
    
    /// 设置源代码
    void SetSourceCode(const std::string& code);
    
    /// 设置配置
    void SetConfig(const ScannerConfig& cfg);
    
    /// 执行扫描
    bool Scan();
    
    /// 获取扫描结果
    const std::vector<CodeFragment>& GetFragments() const;
    
    /// 获取指定类型的片段
    std::vector<CodeFragment> GetFragmentsByType(FragmentType type) const;
    
    /// 获取错误信息
    const std::vector<std::string>& GetErrors() const;
    
    /// 获取警告信息
    const std::vector<std::string>& GetWarnings() const;
    
    /// 重置扫描器
    void Reset();
    
    /// 验证片段完整性
    bool ValidateFragments() const;
    
    /// 获取扫描统计信息
    std::string GetStatistics() const;

private:
    /// 初始化扫描器
    void Initialize();
    
    /// 执行初始分片
    void PerformInitialChunking();
    
    /// 分析片段类型
    FragmentType AnalyzeFragmentType(const std::string& fragment, size_t startPos) const;
    
    /// 检查片段边界的合理性
    bool ValidateFragmentBoundary(size_t startPos, size_t endPos) const;
    
    /// 扩展片段以确保完整性
    size_t ExpandFragmentForCompleteness(size_t startPos, size_t currentEndPos) const;
    
    /// 基于最小单元进行二次切割
    std::vector<CodeFragment> PerformMinimalUnitSplit(const CodeFragment& fragment) const;
    
    /// 检测CHTL语法模式
    bool DetectCHTLPattern(const std::string& text, size_t pos) const;
    
    /// 检测CHTL JS语法模式
    bool DetectCHTLJSPattern(const std::string& text, size_t pos) const;
    
    /// 检测CSS语法模式
    bool DetectCSSPattern(const std::string& text, size_t pos) const;
    
    /// 检测JavaScript语法模式
    bool DetectJSPattern(const std::string& text, size_t pos) const;
    
    /// 查找CHTL/CHTL JS最小单元边界
    size_t FindMinimalUnitBoundary(const std::string& text, size_t startPos, FragmentType type) const;
    
    /// 检查是否为CHTL变量引用（如{{box}}）
    bool IsCHTLVariableReference(const std::string& text, size_t pos) const;
    
    /// 检查是否为CHTL操作符（如->）
    bool IsCHTLOperator(const std::string& text, size_t pos) const;
    
    /// 检查上下文依赖性
    bool CheckContextDependency(const std::string& before, const std::string& after) const;
    
    /// 跳过空白字符
    size_t SkipWhitespace(size_t pos) const;
    
    /// 查找下一个非空白字符
    size_t FindNextNonWhitespace(size_t pos) const;
    
    /// 查找匹配的括号
    size_t FindMatchingBrace(size_t startPos, char openBrace, char closeBrace) const;
    
    /// 更新位置信息
    void UpdatePosition(size_t newPos);
    
    /// 计算行列号
    std::pair<size_t, size_t> CalculateLineColumn(size_t offset) const;
    
    /// 添加片段
    void AddFragment(const std::string& content, FragmentType type, 
                    size_t startPos, size_t endPos);
    
    /// 添加错误信息
    void AddError(const std::string& message);
    
    /// 添加警告信息
    void AddWarning(const std::string& message);
    
    /// 合并连续的同类型片段
    void MergeConsecutiveFragments();
    
    /// 优化片段分布
    void OptimizeFragmentDistribution();
    
    /// 验证单个片段
    bool ValidateFragment(const CodeFragment& fragment) const;
};

/// 片段类型转换为字符串
std::string FragmentTypeToString(FragmentType type);

/// 扫描器工厂类
class ScannerFactory {
public:
    /// 创建默认扫描器
    static std::unique_ptr<CHTLUnifiedScanner> CreateDefault();
    
    /// 创建高性能扫描器
    static std::unique_ptr<CHTLUnifiedScanner> CreateHighPerformance();
    
    /// 创建精确扫描器
    static std::unique_ptr<CHTLUnifiedScanner> CreatePrecision();
    
    /// 创建调试扫描器
    static std::unique_ptr<CHTLUnifiedScanner> CreateDebug();
};

} // namespace CHTL