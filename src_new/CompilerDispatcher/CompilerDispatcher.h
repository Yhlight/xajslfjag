#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../Scanner/UnifiedScanner.h"

namespace CHTL {

class ModuleManager;
class CMODApi;
class CJMODApi;

struct CompilationResult {
    std::string output;
    std::string outputType;
    FragmentType fragmentType;
    bool success = false;
    std::string errorMessage;
    std::unordered_map<std::string, std::string> additionalOutputs;
};

class ICompiler {
public:
    virtual ~ICompiler() = default;
    virtual CompilationResult compile(const CodeFragment& fragment) = 0;
    virtual std::string getCompilerName() const = 0;
    virtual std::vector<FragmentType> getSupportedTypes() const = 0;
};

class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    // 主编译接口
    std::vector<CompilationResult> compileCode(const std::string& code);
    std::vector<CompilationResult> compileFragments(const std::vector<CodeFragment>& fragments);
    CompilationResult compileFragment(const CodeFragment& fragment);
    
    // 编译优化
    void optimizeCompilationOrder(std::vector<CodeFragment>& fragments);
    int getCompilationPriority(FragmentType type);
    
    // 结果合并
    std::string mergeCSS(const std::vector<CompilationResult>& chtlResults);
    std::string mergeJavaScript(const std::vector<CompilationResult>& chtlJSResults);
    
    // 模块管理
    bool loadModule(const std::string& modulePath);
    bool loadCMOD(const std::string& cmodPath);
    bool loadCJMOD(const std::string& cjmodPath);
    
    // 编译器注册
    void registerCompiler(FragmentType type, std::unique_ptr<ICompiler> compiler);
    
    // 查询接口
    std::vector<std::string> getLoadedModules() const;
    
    // 调试和配置
    void enableDebugMode(bool enable);
    
private:
    std::unordered_map<FragmentType, std::unique_ptr<ICompiler>> m_compilers;
    std::unique_ptr<ModuleManager> m_moduleManager;
    std::unique_ptr<CMODApi> m_cmodApi;
    std::unique_ptr<CJMODApi> m_cjmodApi;
    bool m_debugMode;
    
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
};

}