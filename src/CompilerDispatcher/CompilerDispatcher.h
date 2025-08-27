#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "../Scanner/CHTLUnifiedScanner.h"

namespace CHTL {

// 前向声明
class CHTLCompiler;
class CHTLJSCompiler; 
class CSSCompiler;
class JSCompiler;
class Context;

/// 编译器类型枚举
enum class CompilerType {
    CHTL,           // CHTL编译器（手写）
    CHTL_JS,        // CHTL JS编译器（手写）
    CSS,            // CSS编译器（ANTLR）
    JAVASCRIPT      // JavaScript编译器（ANTLR）
};

/// 编译结果结构
struct CompilationResult {
    std::string content;            // 编译结果内容
    CompilerType compilerType;      // 使用的编译器类型
    bool success;                   // 编译是否成功
    std::vector<std::string> errors;        // 错误信息
    std::vector<std::string> warnings;      // 警告信息
    std::unordered_map<std::string, std::string> metadata;  // 元数据
    
    CompilationResult(CompilerType type = CompilerType::CHTL)
        : compilerType(type), success(false) {}
};

/// 调度器配置结构
struct DispatcherConfig {
    bool enableParallelCompilation; // 启用并行编译
    bool enableCrossReference;      // 启用交叉引用
    bool enableOptimization;        // 启用优化
    bool enableDebugInfo;          // 启用调试信息
    size_t maxConcurrency;         // 最大并发数
    
    DispatcherConfig()
        : enableParallelCompilation(true)
        , enableCrossReference(true)
        , enableOptimization(true)
        , enableDebugInfo(false)
        , maxConcurrency(4) {}
};

/// 编译器调度器类
/// 负责协调CHTL、CHTL JS、CSS、JavaScript四种编译器的工作
class CompilerDispatcher {
private:
    /// 统一扫描器
    std::unique_ptr<CHTLUnifiedScanner> scanner;
    
    /// CHTL编译器
    std::unique_ptr<CHTLCompiler> chtlCompiler;
    
    /// CHTL JS编译器
    std::unique_ptr<CHTLJSCompiler> chtlJSCompiler;
    
    /// CSS编译器
    std::unique_ptr<CSSCompiler> cssCompiler;
    
    /// JavaScript编译器
    std::unique_ptr<JSCompiler> jsCompiler;
    
    /// 调度器配置
    DispatcherConfig config;
    
    /// 编译结果缓存
    std::unordered_map<std::string, CompilationResult> resultCache;
    
    /// 错误信息列表
    std::vector<std::string> errors;
    
    /// 警告信息列表
    std::vector<std::string> warnings;

public:
    /// 构造函数
    explicit CompilerDispatcher(const DispatcherConfig& cfg = DispatcherConfig());
    
    /// 析构函数
    ~CompilerDispatcher();
    
    /// 设置配置
    void SetConfig(const DispatcherConfig& cfg);
    
    /// 编译源代码
    CompilationResult Compile(const std::string& sourceCode, std::shared_ptr<Context> context = nullptr);
    
    /// 编译指定文件
    CompilationResult CompileFile(const std::string& filePath, std::shared_ptr<Context> context = nullptr);
    
    /// 批量编译文件
    std::vector<CompilationResult> CompileFiles(const std::vector<std::string>& filePaths,
                                               std::shared_ptr<Context> context = nullptr);
    
    /// 获取错误信息
    const std::vector<std::string>& GetErrors() const;
    
    /// 获取警告信息
    const std::vector<std::string>& GetWarnings() const;
    
    /// 清除缓存
    void ClearCache();
    
    /// 获取编译统计信息
    std::string GetStatistics() const;
    
    /// 设置自定义编译器
    void SetCHTLCompiler(std::unique_ptr<CHTLCompiler> compiler);
    void SetCHTLJSCompiler(std::unique_ptr<CHTLJSCompiler> compiler);
    void SetCSSCompiler(std::unique_ptr<CSSCompiler> compiler);
    void SetJSCompiler(std::unique_ptr<JSCompiler> compiler);

private:
    /// 初始化编译器
    void InitializeCompilers();
    
    /// 分发编译任务
    std::vector<CompilationResult> DispatchCompilation(const std::vector<CodeFragment>& fragments,
                                                       std::shared_ptr<Context> context);
    
    /// 编译单个片段
    CompilationResult CompileFragment(const CodeFragment& fragment, std::shared_ptr<Context> context);
    
    /// 合并编译结果
    CompilationResult MergeResults(const std::vector<CompilationResult>& results);
    
    /// 处理交叉引用
    void ProcessCrossReferences(std::vector<CompilationResult>& results);
    
    /// 优化编译结果
    void OptimizeResults(std::vector<CompilationResult>& results);
    
    /// 生成最终HTML
    std::string GenerateFinalHTML(const std::vector<CompilationResult>& results);
    
    /// 添加错误信息
    void AddError(const std::string& message);
    
    /// 添加警告信息
    void AddWarning(const std::string& message);
    
    /// 并行编译片段
    std::vector<CompilationResult> ParallelCompileFragments(const std::vector<CodeFragment>& fragments,
                                                           std::shared_ptr<Context> context);
    
    /// 串行编译片段
    std::vector<CompilationResult> SerialCompileFragments(const std::vector<CodeFragment>& fragments,
                                                         std::shared_ptr<Context> context);
    
    /// 验证编译结果
    bool ValidateResults(const std::vector<CompilationResult>& results);
    
    /// 生成缓存键
    std::string GenerateCacheKey(const std::string& content, CompilerType type);
};

/// 编译器接口基类
class ICompiler {
public:
    virtual ~ICompiler() = default;
    
    /// 编译方法
    virtual CompilationResult Compile(const std::string& source, std::shared_ptr<Context> context) = 0;
    
    /// 获取编译器类型
    virtual CompilerType GetType() const = 0;
    
    /// 获取编译器名称
    virtual std::string GetName() const = 0;
    
    /// 检查是否支持指定的代码片段
    virtual bool CanCompile(const CodeFragment& fragment) const = 0;
};

/// CHTL编译器类（手写实现）
class CHTLCompiler : public ICompiler {
public:
    CHTLCompiler();
    ~CHTLCompiler() override;
    
    CompilationResult Compile(const std::string& source, std::shared_ptr<Context> context) override;
    CompilerType GetType() const override { return CompilerType::CHTL; }
    std::string GetName() const override { return "CHTL手写编译器"; }
    bool CanCompile(const CodeFragment& fragment) const override;

private:
    // CHTL编译器的具体实现将在后续实现
};

/// CHTL JS编译器类（手写实现）
class CHTLJSCompiler : public ICompiler {
public:
    CHTLJSCompiler();
    ~CHTLJSCompiler() override;
    
    CompilationResult Compile(const std::string& source, std::shared_ptr<Context> context) override;
    CompilerType GetType() const override { return CompilerType::CHTL_JS; }
    std::string GetName() const override { return "CHTL JS手写编译器"; }
    bool CanCompile(const CodeFragment& fragment) const override;

private:
    // CHTL JS编译器的具体实现将在后续实现
};

/// CSS编译器类（基于ANTLR）
class CSSCompiler : public ICompiler {
public:
    CSSCompiler();
    ~CSSCompiler() override;
    
    CompilationResult Compile(const std::string& source, std::shared_ptr<Context> context) override;
    CompilerType GetType() const override { return CompilerType::CSS; }
    std::string GetName() const override { return "CSS ANTLR编译器"; }
    bool CanCompile(const CodeFragment& fragment) const override;

private:
    // 基于ANTLR的CSS编译器实现
};

/// JavaScript编译器类（基于ANTLR）
class JSCompiler : public ICompiler {
public:
    JSCompiler();
    ~JSCompiler() override;
    
    CompilationResult Compile(const std::string& source, std::shared_ptr<Context> context) override;
    CompilerType GetType() const override { return CompilerType::JAVASCRIPT; }
    std::string GetName() const override { return "JavaScript ANTLR编译器"; }
    bool CanCompile(const CodeFragment& fragment) const override;

private:
    // 基于ANTLR的JavaScript编译器实现
};

/// 编译器工厂类
class CompilerFactory {
public:
    /// 创建默认调度器
    static std::unique_ptr<CompilerDispatcher> CreateDefault();
    
    /// 创建高性能调度器
    static std::unique_ptr<CompilerDispatcher> CreateHighPerformance();
    
    /// 创建调试调度器
    static std::unique_ptr<CompilerDispatcher> CreateDebug();
    
    /// 创建指定类型的编译器
    static std::unique_ptr<ICompiler> CreateCompiler(CompilerType type);
};

/// 编译器类型转换为字符串
std::string CompilerTypeToString(CompilerType type);

} // namespace CHTL