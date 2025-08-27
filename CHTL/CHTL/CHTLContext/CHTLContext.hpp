#pragma once
#include "../CHTLLexer/GlobalMap.hpp"
#include "../CHTLState/CHTLState.hpp"
#include "../CHTLNode/BaseNode.hpp"
#include <memory>
#include <stack>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * 编译阶段枚举
 */
enum class CompilationPhase {
    LEXICAL_ANALYSIS,     // 词法分析
    SYNTAX_ANALYSIS,      // 语法分析
    SEMANTIC_ANALYSIS,    // 语义分析
    CODE_GENERATION,      // 代码生成
    OPTIMIZATION,         // 优化
    OUTPUT_GENERATION     // 输出生成
};

/**
 * 编译上下文信息
 */
struct CompilationContext {
    std::string sourceFile;
    std::string workingDirectory;
    std::string outputDirectory;
    std::vector<std::string> includePaths;
    std::unordered_map<std::string, std::string> defines;
    CompilationPhase currentPhase;
    bool debugMode;
    bool optimizationEnabled;
    
    CompilationContext() : currentPhase(CompilationPhase::LEXICAL_ANALYSIS),
                          debugMode(false), optimizationEnabled(false) {}
};

/**
 * 作用域管理器
 * 管理变量和符号的作用域
 */
class ScopeManager {
private:
    struct Scope {
        std::unordered_map<std::string, std::shared_ptr<BaseNode>> symbols;
        std::unordered_map<std::string, std::string> variables;
        std::string scopeName;
        ContextType scopeType;
        
        Scope(const std::string& name, ContextType type) 
            : scopeName(name), scopeType(type) {}
    };
    
    std::stack<Scope> scopes;
    
public:
    // 作用域管理
    void pushScope(const std::string& name, ContextType type);
    void popScope();
    size_t getScopeDepth() const;
    
    // 符号管理
    void defineSymbol(const std::string& name, std::shared_ptr<BaseNode> node);
    std::shared_ptr<BaseNode> lookupSymbol(const std::string& name) const;
    bool hasSymbol(const std::string& name) const;
    void removeSymbol(const std::string& name);
    
    // 变量管理
    void setVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    void removeVariable(const std::string& name);
    
    // 查询方法
    std::string getCurrentScopeName() const;
    ContextType getCurrentScopeType() const;
    std::vector<std::string> getAllSymbols() const;
    std::vector<std::string> getAllVariables() const;
    
    // 调试
    void dumpScopes() const;
    void clear();
};

/**
 * 依赖管理器
 * 管理模块间的依赖关系
 */
class DependencyManager {
private:
    struct Dependency {
        std::string path;
        std::string alias;
        std::string type;
        bool isResolved;
        std::shared_ptr<BaseNode> resolvedNode;
        
        Dependency(const std::string& p, const std::string& a, const std::string& t)
            : path(p), alias(a), type(t), isResolved(false) {}
    };
    
    std::unordered_map<std::string, Dependency> dependencies;
    std::vector<std::string> loadOrder;
    std::unordered_set<std::string> circularDependencies;

public:
    // 依赖注册
    void addDependency(const std::string& name, const std::string& path, 
                      const std::string& alias, const std::string& type);
    void removeDependency(const std::string& name);
    bool hasDependency(const std::string& name) const;
    
    // 依赖解析
    bool resolveDependency(const std::string& name, std::shared_ptr<BaseNode> node);
    std::shared_ptr<BaseNode> getDependency(const std::string& name) const;
    bool isDependencyResolved(const std::string& name) const;
    
    // 循环依赖检测
    bool hasCircularDependencies() const;
    std::vector<std::string> getCircularDependencies() const;
    void detectCircularDependencies();
    
    // 加载顺序
    std::vector<std::string> getLoadOrder() const;
    void computeLoadOrder();
    
    // 工具方法
    void clear();
    size_t getDependencyCount() const;
    std::vector<std::string> getUnresolvedDependencies() const;
};

/**
 * CHTL编译上下文
 * 管理整个编译过程的上下文信息
 */
class CHTLContext {
private:
    CompilationContext compilationCtx;
    std::unique_ptr<ScopeManager> scopeManager;
    std::unique_ptr<DependencyManager> dependencyManager;
    std::unique_ptr<CHTLState> state;
    std::shared_ptr<GlobalMap> globalMap;
    
    // AST根节点
    std::shared_ptr<BaseNode> rootNode;
    
    // 错误和警告收集
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 输出管理
    std::string htmlOutput;
    std::string cssOutput;
    std::string jsOutput;
    
    // 统计信息
    struct Statistics {
        size_t nodesCreated;
        size_t symbolsResolved;
        size_t dependenciesLoaded;
        size_t errorsGenerated;
        size_t warningsGenerated;
        size_t compilationTime; // 毫秒
        
        Statistics() : nodesCreated(0), symbolsResolved(0), dependenciesLoaded(0),
                      errorsGenerated(0), warningsGenerated(0), compilationTime(0) {}
    } statistics;

public:
    CHTLContext();
    explicit CHTLContext(const CompilationContext& ctx);
    ~CHTLContext();
    
    // 编译上下文管理
    void setCompilationContext(const CompilationContext& ctx);
    const CompilationContext& getCompilationContext() const;
    CompilationContext& getCompilationContext();
    
    // 阶段管理
    void setCurrentPhase(CompilationPhase phase);
    CompilationPhase getCurrentPhase() const;
    void advancePhase();
    
    // 状态管理
    CHTLState& getState();
    const CHTLState& getState() const;
    
    // 作用域管理
    ScopeManager& getScopeManager();
    const ScopeManager& getScopeManager() const;
    
    // 依赖管理
    DependencyManager& getDependencyManager();
    const DependencyManager& getDependencyManager() const;
    
    // 全局映射
    GlobalMap& getGlobalMap();
    const GlobalMap& getGlobalMap() const;
    
    // AST管理
    void setRootNode(std::shared_ptr<BaseNode> root);
    std::shared_ptr<BaseNode> getRootNode() const;
    
    // 错误和警告管理
    void reportError(const std::string& error);
    void reportWarning(const std::string& warning);
    const std::vector<std::string>& getErrors() const;
    const std::vector<std::string>& getWarnings() const;
    bool hasErrors() const;
    bool hasWarnings() const;
    void clearErrorsAndWarnings();
    
    // 输出管理
    void setHTMLOutput(const std::string& html);
    void setCSSOutput(const std::string& css);
    void setJSOutput(const std::string& js);
    const std::string& getHTMLOutput() const;
    const std::string& getCSSOutput() const;
    const std::string& getJSOutput() const;
    
    // 输出合并
    std::string generateCombinedOutput() const;
    void writeOutputToFiles() const;
    
    // 工作目录和文件管理
    void setWorkingDirectory(const std::string& dir);
    void setOutputDirectory(const std::string& dir);
    void addIncludePath(const std::string& path);
    std::string resolveIncludePath(const std::string& relativePath) const;
    
    // 预定义宏管理
    void defineSymbol(const std::string& name, const std::string& value);
    std::string getDefinedSymbol(const std::string& name) const;
    bool hasDefinedSymbol(const std::string& name) const;
    
    // 统计信息
    void incrementNodeCount();
    void incrementSymbolCount();
    void incrementDependencyCount();
    void setCompilationTime(size_t timeMs);
    const Statistics& getStatistics() const;
    
    // 调试和监控
    void enableDebugMode(bool enable);
    void enableOptimization(bool enable);
    bool isDebugMode() const;
    bool isOptimizationEnabled() const;
    
    // 上下文状态
    bool isValid() const;
    void validate();
    
    // 重置和清理
    void reset();
    void clear();
    
    // 调试输出
    void dumpContext() const;
    std::string getContextSummary() const;
    
    // 配置加载
    void loadConfiguration(const std::string& configFile);
    void saveConfiguration(const std::string& configFile) const;
};

/**
 * 上下文工厂
 * 创建不同配置的上下文实例
 */
class ContextFactory {
public:
    static std::unique_ptr<CHTLContext> createDefaultContext();
    static std::unique_ptr<CHTLContext> createProductionContext();
    static std::unique_ptr<CHTLContext> createDevelopmentContext();
    static std::unique_ptr<CHTLContext> createDebugContext();
    
    // 配置方法
    static void configureForProduction(CHTLContext& context);
    static void configureForDevelopment(CHTLContext& context);
    static void configureForDebugging(CHTLContext& context);
};

/**
 * 全局上下文管理器
 * 全局上下文管理
 */
class GlobalContextManager {
private:
    static std::stack<std::shared_ptr<CHTLContext>> contextStack;
    static std::shared_ptr<CHTLContext> currentContext;

public:
    // 上下文栈管理
    static void pushContext(std::shared_ptr<CHTLContext> context);
    static void popContext();
    static std::shared_ptr<CHTLContext> getCurrentContext();
    static size_t getContextDepth();
    
    // 快捷访问
    static CHTLState& getCurrentState();
    static GlobalMap& getCurrentGlobalMap();
    static ScopeManager& getCurrentScopeManager();
    static DependencyManager& getCurrentDependencyManager();
    
    // 清理
    static void clearAll();
};

}