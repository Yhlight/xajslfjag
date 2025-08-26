#ifndef COMPILER_DISPATCHER_H
#define COMPILER_DISPATCHER_H

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "../scanner/CHTLUnifiedScanner.h"

namespace chtl {

// 前向声明
class CHTLCompiler;
class CHTLJSCompiler;
class CSSCompiler;
class JSCompiler;

// 编译结果结构
struct CompileResult {
    bool success;
    std::string html;
    std::string css;
    std::string javascript;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    CompileResult() : success(true) {}
};

// 编译器接口
class ICompiler {
public:
    virtual ~ICompiler() = default;
    virtual CompileResult compile(const std::vector<CodeFragment>& fragments) = 0;
    virtual bool canHandle(CodeFragmentType type) const = 0;
    virtual std::string getName() const = 0;
};

// 调度器配置
struct DispatcherConfig {
    bool enableParallelCompilation = false;    // 启用并行编译
    bool enableCaching = true;                 // 启用编译缓存
    bool strictMode = false;                   // 严格模式
    size_t maxWorkerThreads = 4;               // 最大工作线程数
};

// 编译器调度器
class CompilerDispatcher {
public:
    CompilerDispatcher();
    explicit CompilerDispatcher(const DispatcherConfig& config);
    ~CompilerDispatcher();
    
    // 注册编译器
    void registerCompiler(std::unique_ptr<ICompiler> compiler);
    
    // 主要调度接口
    CompileResult dispatch(const std::vector<CodeFragment>& fragments);
    CompileResult dispatchFile(const std::string& filePath);
    
    // 获取特定编译器
    ICompiler* getCompiler(const std::string& name) const;
    ICompiler* getCompilerForType(CodeFragmentType type) const;
    
    // 缓存管理
    void clearCache();
    bool isCacheEnabled() const { return config.enableCaching; }
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&)> handler);
    
private:
    // 内部调度方法
    std::unordered_map<CodeFragmentType, std::vector<CodeFragment>> 
        groupFragmentsByType(const std::vector<CodeFragment>& fragments);
    
    CompileResult mergeResults(const std::vector<CompileResult>& results);
    
    // 并行编译支持
    std::vector<CompileResult> compileParallel(
        const std::unordered_map<CodeFragmentType, std::vector<CodeFragment>>& groupedFragments
    );
    
    // 串行编译
    std::vector<CompileResult> compileSerial(
        const std::unordered_map<CodeFragmentType, std::vector<CodeFragment>>& groupedFragments
    );
    
    // 缓存支持
    std::string getCacheKey(const std::vector<CodeFragment>& fragments) const;
    bool getCachedResult(const std::string& key, CompileResult& result) const;
    void cacheResult(const std::string& key, const CompileResult& result);
    
private:
    DispatcherConfig config;
    std::vector<std::unique_ptr<ICompiler>> compilers;
    std::unique_ptr<CHTLUnifiedScanner> scanner;
    
    // 类型到编译器的映射
    std::unordered_map<CodeFragmentType, ICompiler*> typeToCompiler;
    
    // 缓存
    mutable std::unordered_map<std::string, CompileResult> cache;
    
    // 错误处理器
    std::function<void(const std::string&)> errorHandler;
};

// CHTL编译器包装器
class CHTLCompilerWrapper : public ICompiler {
public:
    CHTLCompilerWrapper();
    ~CHTLCompilerWrapper() override;
    
    CompileResult compile(const std::vector<CodeFragment>& fragments) override;
    bool canHandle(CodeFragmentType type) const override;
    std::string getName() const override { return "CHTL Compiler"; }
    
private:
    std::unique_ptr<CHTLCompiler> compiler;
};

// CHTL JS编译器包装器
class CHTLJSCompilerWrapper : public ICompiler {
public:
    CHTLJSCompilerWrapper();
    ~CHTLJSCompilerWrapper() override;
    
    CompileResult compile(const std::vector<CodeFragment>& fragments) override;
    bool canHandle(CodeFragmentType type) const override;
    std::string getName() const override { return "CHTL JS Compiler"; }
    
private:
    std::unique_ptr<CHTLJSCompiler> compiler;
};

// CSS编译器包装器
class CSSCompilerWrapper : public ICompiler {
public:
    CSSCompilerWrapper();
    ~CSSCompilerWrapper() override;
    
    CompileResult compile(const std::vector<CodeFragment>& fragments) override;
    bool canHandle(CodeFragmentType type) const override;
    std::string getName() const override { return "CSS Compiler"; }
    
private:
    // 这里会使用ANTLR生成的CSS编译器
    void* cssParser;  // 实际类型会是ANTLR生成的parser
};

// JavaScript编译器包装器
class JSCompilerWrapper : public ICompiler {
public:
    JSCompilerWrapper();
    ~JSCompilerWrapper() override;
    
    CompileResult compile(const std::vector<CodeFragment>& fragments) override;
    bool canHandle(CodeFragmentType type) const override;
    std::string getName() const override { return "JavaScript Compiler"; }
    
private:
    // 这里会使用ANTLR生成的JS编译器
    void* jsParser;  // 实际类型会是ANTLR生成的parser
};

} // namespace chtl

#endif // COMPILER_DISPATCHER_H