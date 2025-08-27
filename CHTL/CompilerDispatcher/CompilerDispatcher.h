#ifndef CHTL_COMPILER_DISPATCHER_H
#define CHTL_COMPILER_DISPATCHER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../Scanner/Scanner.h"

namespace CHTL {

// 前向声明
class CHTLCompiler;
class CHTLJSCompiler;
class CSSCompiler;
class JSCompiler;

// 编译结果
struct CompileResult {
    bool success;
    std::string output;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 各部分的输出
    std::string htmlOutput;
    std::string cssOutput;
    std::string jsOutput;
    
    CompileResult() : success(true) {}
};

// 编译器配置
struct CompilerConfig {
    bool debugMode;
    bool generateSourceMap;
    bool minifyOutput;
    std::string outputPath;
    std::string modulePath;
    
    CompilerConfig() 
        : debugMode(false), generateSourceMap(false), minifyOutput(false) {}
};

// 编译器调度器
class CompilerDispatcher {
private:
    std::unique_ptr<Scanner> scanner;
    std::unique_ptr<CHTLCompiler> chtlCompiler;
    std::unique_ptr<CHTLJSCompiler> chtljsCompiler;
    std::unique_ptr<CSSCompiler> cssCompiler;
    std::unique_ptr<JSCompiler> jsCompiler;
    
    CompilerConfig config;
    std::vector<CodeFragment> fragments;
    
    // 片段分发
    void dispatchFragment(const CodeFragment& fragment, CompileResult& result);
    void dispatchToCHTL(const CodeFragment& fragment, CompileResult& result);
    void dispatchToCHTLJS(const CodeFragment& fragment, CompileResult& result);
    void dispatchToCSS(const CodeFragment& fragment, CompileResult& result);
    void dispatchToJS(const CodeFragment& fragment, CompileResult& result);
    
    // 结果合并
    void mergeResults(CompileResult& result);
    std::string generateHTML(const CompileResult& result);
    
    // 错误处理
    void handleError(const std::string& error, CompileResult& result);
    void handleWarning(const std::string& warning, CompileResult& result);
    
    // 依赖管理
    std::unordered_map<std::string, std::string> resolvedImports;
    void resolveImports();
    
public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    // 配置
    void setConfig(const CompilerConfig& cfg) { config = cfg; }
    const CompilerConfig& getConfig() const { return config; }
    
    // 编译接口
    CompileResult compile(const std::string& source);
    CompileResult compileFile(const std::string& filePath);
    
    // 模块编译
    CompileResult compileCMOD(const std::string& modulePath);
    CompileResult compileCJMOD(const std::string& modulePath);
    
    // 获取编译器实例（用于高级配置）
    CHTLCompiler* getCHTLCompiler() { return chtlCompiler.get(); }
    CHTLJSCompiler* getCHTLJSCompiler() { return chtljsCompiler.get(); }
    CSSCompiler* getCSSCompiler() { return cssCompiler.get(); }
    JSCompiler* getJSCompiler() { return jsCompiler.get(); }
    
    // 调试
    void dumpIntermediateResults() const;
};

// 编译器基类
class BaseCompiler {
protected:
    CompilerConfig config;
    
public:
    virtual ~BaseCompiler() = default;
    
    virtual void setConfig(const CompilerConfig& cfg) { config = cfg; }
    virtual CompileResult compile(const std::string& source) = 0;
    virtual std::string getName() const = 0;
};

} // namespace CHTL

#endif // CHTL_COMPILER_DISPATCHER_H