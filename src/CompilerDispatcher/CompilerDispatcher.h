#ifndef COMPILER_DISPATCHER_H
#define COMPILER_DISPATCHER_H

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>

namespace CHTL {

// 前向声明
class CHTLCompiler;
class CHTLJSCompiler;
class CSSCompiler;
class JavaScriptCompiler;
class CHTLUnifiedScanner;
struct CodeFragment;

// 编译器类型
enum class CompilerType {
    CHTL,
    CHTLJS,
    CSS,
    JAVASCRIPT,
    MIXED
};

// 编译选项
struct CompileOptions {
    std::string inputFile;
    std::string outputFile;
    std::string outputDir = "./output";
    bool generateSourceMap = true;
    bool minify = false;
    bool prettify = true;
    bool enableDebugInfo = false;
    std::string targetVersion = "ES6";
    std::string encoding = "UTF-8";
    std::unordered_map<std::string, std::string> customConfig;
};

// 编译结果
struct CompileResult {
    bool success = false;
    std::string outputPath;
    std::string htmlOutput;
    std::string cssOutput;
    std::string jsOutput;
    std::string sourceMap;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::vector<std::string> includedFiles;
    size_t processedFragments = 0;
    double compilationTime = 0.0;
};

// 片段路由信息
struct FragmentRoute {
    CompilerType compiler;
    std::string context;
    size_t priority = 0;
};

// 编译器调度器
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    // 初始化调度器
    bool initialize();
    
    // 设置编译选项
    void setOptions(const CompileOptions& options) { options_ = options; }
    const CompileOptions& getOptions() const { return options_; }
    
    // 编译文件
    CompileResult compile(const std::string& inputFile);
    
    // 编译字符串
    CompileResult compileString(const std::string& content, const std::string& filename = "inline");
    
    // 批量编译
    std::vector<CompileResult> compileBatch(const std::vector<std::string>& files);
    
    // 注册编译器
    void registerCompiler(CompilerType type, std::shared_ptr<void> compiler);
    
    // 获取编译器
    template<typename T>
    std::shared_ptr<T> getCompiler(CompilerType type) {
        auto it = compilers_.find(type);
        if (it != compilers_.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }
    
    // 设置片段路由规则
    void setFragmentRoute(const std::string& pattern, const FragmentRoute& route);
    
    // 设置错误处理器
    void setErrorHandler(std::function<void(const std::string&)> handler) {
        errorHandler_ = handler;
    }
    
    // 设置警告处理器
    void setWarningHandler(std::function<void(const std::string&)> handler) {
        warningHandler_ = handler;
    }
    
    // 设置进度回调
    void setProgressCallback(std::function<void(size_t current, size_t total)> callback) {
        progressCallback_ = callback;
    }

private:
    CompileOptions options_;
    std::unordered_map<CompilerType, std::shared_ptr<void>> compilers_;
    std::unique_ptr<CHTLUnifiedScanner> scanner_;
    std::unordered_map<std::string, FragmentRoute> fragmentRoutes_;
    
    // 回调函数
    std::function<void(const std::string&)> errorHandler_;
    std::function<void(const std::string&)> warningHandler_;
    std::function<void(size_t, size_t)> progressCallback_;
    
    // 内部方法
    CompileResult doCompile(const std::string& content, const std::string& filename);
    void dispatchFragments(const std::vector<CodeFragment>& fragments, CompileResult& result);
    CompilerType determineCompiler(const CodeFragment& fragment);
    void mergeResults(CompileResult& mainResult, const CompileResult& fragmentResult);
    
    // 编译各种类型的完整代码
    CompileResult compileCHTL(const std::string& code);
    CompileResult compileCHTLJS(const std::string& code);
    CompileResult compileCSS(const std::string& code);
    CompileResult compileJavaScript(const std::string& code);
    void generateOutput(CompileResult& result);
    void reportError(const std::string& error, CompileResult& result);
    void reportWarning(const std::string& warning, CompileResult& result);
};

// 编译器工厂
class CompilerFactory {
public:
    static std::shared_ptr<CompilerDispatcher> createDispatcher();
    static std::shared_ptr<CHTLCompiler> createCHTLCompiler();
    static std::shared_ptr<CHTLJSCompiler> createCHTLJSCompiler();
    static std::shared_ptr<CSSCompiler> createCSSCompiler();
    static std::shared_ptr<JavaScriptCompiler> createJavaScriptCompiler();
};

// 编译器管理器（单例）
class CompilerManager {
public:
    static CompilerManager& getInstance() {
        static CompilerManager instance;
        return instance;
    }
    
    // 获取默认调度器
    std::shared_ptr<CompilerDispatcher> getDefaultDispatcher();
    
    // 创建新的调度器
    std::shared_ptr<CompilerDispatcher> createDispatcher();
    
    // 注册调度器
    void registerDispatcher(const std::string& name, std::shared_ptr<CompilerDispatcher> dispatcher);
    
    // 获取命名调度器
    std::shared_ptr<CompilerDispatcher> getDispatcher(const std::string& name);
    
    // 设置全局选项
    void setGlobalOptions(const CompileOptions& options) { globalOptions_ = options; }
    const CompileOptions& getGlobalOptions() const { return globalOptions_; }
    
private:
    CompilerManager();
    ~CompilerManager() = default;
    CompilerManager(const CompilerManager&) = delete;
    CompilerManager& operator=(const CompilerManager&) = delete;
    
    std::shared_ptr<CompilerDispatcher> defaultDispatcher_;
    std::unordered_map<std::string, std::shared_ptr<CompilerDispatcher>> dispatchers_;
    CompileOptions globalOptions_;
};

// 编译器接口基类
class ICompiler {
public:
    virtual ~ICompiler() = default;
    
    // 编译代码片段
    virtual CompileResult compile(const std::string& code, const CompileOptions& options) = 0;
    
    // 验证代码
    virtual bool validate(const std::string& code) = 0;
    
    // 获取编译器名称
    virtual std::string getName() const = 0;
    
    // 获取支持的版本
    virtual std::string getVersion() const = 0;
};

// CHTL编译器接口
class CHTLCompiler : public ICompiler {
public:
    virtual ~CHTLCompiler() = default;
    
    // 设置命名空间管理器
    virtual void setNamespaceManager(std::shared_ptr<void> manager) = 0;
    
    // 设置导入解析器
    virtual void setImportResolver(std::shared_ptr<void> resolver) = 0;
    
    // 设置选择器自动化
    virtual void setSelectorAutomation(std::shared_ptr<void> automation) = 0;
};

// CHTL JS编译器接口
class CHTLJSCompiler : public ICompiler {
public:
    virtual ~CHTLJSCompiler() = default;
    
    // 设置CJMOD加载器
    virtual void setCJMODLoader(std::shared_ptr<void> loader) = 0;
    
    // 设置虚拟对象管理器
    virtual void setVirtualObjectManager(std::shared_ptr<void> manager) = 0;
};

// CSS编译器接口
class CSSCompiler : public ICompiler {
public:
    virtual ~CSSCompiler() = default;
    
    // 设置CSS预处理器
    virtual void setPreprocessor(const std::string& preprocessor) = 0;
    
    // 设置自动前缀
    virtual void setAutoprefixer(bool enable) = 0;
};

// JavaScript编译器接口
class JavaScriptCompiler : public ICompiler {
public:
    virtual ~JavaScriptCompiler() = default;
    
    // 设置目标版本
    virtual void setTargetVersion(const std::string& version) = 0;
    
    // 设置模块系统
    virtual void setModuleSystem(const std::string& system) = 0;
};

} // namespace CHTL

#endif // COMPILER_DISPATCHER_H