#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../Scanner/CHTLUnifiedScanner.h"

namespace CHTL {

// 前向声明
class CHTLCompiler;
class CHTLJSCompiler;
class CSSCompiler;
class JSCompiler;

// 编译结果
struct CompilationResult {
    bool success;
    std::string output;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    CompilationResult(bool s = false) : success(s) {}
};

using CompilationResultPtr = std::shared_ptr<CompilationResult>;

// CHTL编译器调度器 - 协调各个编译器工作
class CHTLCompilerDispatcher {
public:
    CHTLCompilerDispatcher();
    ~CHTLCompilerDispatcher();
    
    // 主要编译接口
    CompilationResultPtr CompileFile(const std::string& filePath);
    CompilationResultPtr CompileString(const std::string& input);
    
    // 分步编译接口
    std::vector<CodeFragmentPtr> ScanInput(const std::string& input);
    CompilationResultPtr CompileFragments(const std::vector<CodeFragmentPtr>& fragments);
    
    // 配置选项
    void SetOutputFormat(const std::string& format);        // html, html5
    void SetOptimizationLevel(int level);                   // 0-3
    void EnableDebugMode(bool enable = true);
    void SetWorkingDirectory(const std::string& dir);
    
    // 模块管理
    void AddModulePath(const std::string& path);
    void SetOfficialModulePath(const std::string& path);
    
    // 错误处理
    bool HasErrors() const;
    const std::vector<std::string>& GetErrors() const;
    const std::vector<std::string>& GetWarnings() const;
    void ClearErrors();
    
    // 统计信息
    size_t GetCompiledFragments() const;
    double GetLastCompilationTime() const;
    void PrintCompilationStats() const;
    
private:
    // 核心组件
    std::unique_ptr<CHTLUnifiedScanner> scanner_;
    // 暂时注释编译器实例，待实现后再启用
    // std::unique_ptr<CHTLCompiler> chtlCompiler_;
    // std::unique_ptr<CHTLJSCompiler> chtljsCompiler_;
    // std::unique_ptr<CSSCompiler> cssCompiler_;
    // std::unique_ptr<JSCompiler> jsCompiler_;
    
    // 配置
    std::string outputFormat_;
    int optimizationLevel_;
    bool debugMode_;
    std::string workingDirectory_;
    std::vector<std::string> modulePaths_;
    std::string officialModulePath_;
    
    // 状态和统计
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    size_t compiledFragments_;
    double lastCompilationTime_;
    
    // 内部编译方法
    CompilationResultPtr CompileInternal(const std::string& input);
    CompilationResultPtr CompileCHTLFragment(const CodeFragmentPtr& fragment);
    CompilationResultPtr CompileCHTLJSFragment(const CodeFragmentPtr& fragment);
    CompilationResultPtr CompileCSSFragment(const CodeFragmentPtr& fragment);
    CompilationResultPtr CompileJSFragment(const CodeFragmentPtr& fragment);
    
    // 结果合并
    CompilationResultPtr MergeResults(const std::vector<CompilationResultPtr>& results);
    std::string CombineOutputs(const std::vector<CompilationResultPtr>& results);
    
    // 优化处理
    std::string OptimizeOutput(const std::string& output);
    void ApplyMinification(std::string& output);
    void RemoveComments(std::string& output);
    void CompressWhitespace(std::string& output);
    
    // 文件处理
    std::string ReadFile(const std::string& filePath);
    bool WriteFile(const std::string& filePath, const std::string& content);
    std::string ResolveIncludePath(const std::string& path);
    
    // 错误处理
    void ReportError(const std::string& message);
    void ReportWarning(const std::string& message);
    void CollectCompilerErrors();
    
    // 初始化
    void InitializeCompilers();
    void ConfigureCompilers();
    
    // 时间测量
    double GetCurrentTime() const;
    void StartTimer();
    void EndTimer();
    double timerStart_;
};

} // namespace CHTL