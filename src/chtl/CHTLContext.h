#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <memory>
#include <vector>
#include <stack>
#include <unordered_map>
#include "CHTLToken.h"
#include "CHTLState.h"
#include "CHTLGlobalMap.h"

namespace chtl {

// 前向声明
class ASTNode;
class CHTLLexer;
class CHTLParser;
class CHTLGenerator;
class ConfigurationNode;

// 编译选项
struct CompileOptions {
    bool strictMode = false;              // 严格模式
    bool enableDebug = false;             // 调试模式
    bool generateSourceMap = false;       // 生成源映射
    bool enableOptimization = true;       // 启用优化
    bool allowUnquotedLiterals = true;   // 允许无修饰字面量
    bool enableCEEquality = true;         // 启用CE对等式（: = 等价）
    
    // 自动化开关
    bool disableStyleAutoAddClass = false;
    bool disableStyleAutoAddId = false;
    bool disableScriptAutoAddClass = true;
    bool disableScriptAutoAddId = true;
    
    // 默认配置
    bool disableNameGroup = false;
    bool disableCustomOriginType = false;
    bool disableDefaultNamespace = false;
    
    // 索引配置
    int indexInitialCount = 0;
    int optionCount = 3;  // 组选项数量限制
};

// 错误信息
struct CompileError {
    enum class ErrorType {
        SYNTAX_ERROR,
        SEMANTIC_ERROR,
        TYPE_ERROR,
        CONSTRAINT_ERROR,
        IMPORT_ERROR,
        NAMESPACE_ERROR,
        WARNING
    };
    
    ErrorType type;
    std::string message;
    std::string file;
    size_t line;
    size_t column;
    std::string context;  // 错误上下文代码片段
    
    CompileError(ErrorType t, const std::string& msg, const std::string& f = "", 
                size_t l = 0, size_t c = 0)
        : type(t), message(msg), file(f), line(l), column(c) {}
};

// 编译上下文
class CHTLContext {
public:
    CHTLContext();
    explicit CHTLContext(const CompileOptions& options);
    ~CHTLContext();
    
    // 选项管理
    void setOptions(const CompileOptions& options);
    const CompileOptions& getOptions() const { return options; }
    
    // 文件管理
    void setCurrentFile(const std::string& file);
    std::string getCurrentFile() const { return currentFile; }
    void addSourceFile(const std::string& file);
    std::vector<std::string> getSourceFiles() const { return sourceFiles; }
    
    // 全局符号表访问
    CHTLGlobalMap& getGlobalMap() { return *globalMap; }
    const CHTLGlobalMap& getGlobalMap() const { return *globalMap; }
    
    // 状态管理器访问
    CHTLStateManager& getStateManager() { return *stateManager; }
    const CHTLStateManager& getStateManager() const { return *stateManager; }
    
    // 约束管理器访问
    ConstraintManager& getConstraintManager() { return *constraintManager; }
    const ConstraintManager& getConstraintManager() const { return *constraintManager; }
    
    // 配置管理
    void setActiveConfiguration(std::shared_ptr<ConfigurationNode> config);
    std::shared_ptr<ConfigurationNode> getActiveConfiguration() const;
    bool hasConfiguration() const { return activeConfig != nullptr; }
    
    // 错误和警告管理
    void addError(const CompileError& error);
    void addWarning(const std::string& message, size_t line = 0, size_t column = 0);
    std::vector<CompileError> getErrors() const { return errors; }
    std::vector<CompileError> getWarnings() const { return warnings; }
    bool hasErrors() const { return !errors.empty(); }
    void clearErrors() { errors.clear(); warnings.clear(); }
    
    // 作用域管理
    void enterScope(const std::string& name);
    void exitScope();
    std::string getCurrentScope() const;
    std::vector<std::string> getScopeStack() const { return scopeStack; }
    
    // 元素路径管理（用于自动化类名/ID）
    void pushElement(const std::string& element);
    void popElement();
    std::string getCurrentElementPath() const;
    
    // 导入路径解析
    std::string resolveImportPath(const std::string& path, const std::string& type) const;
    std::string getModulePath() const { return modulePath; }
    void setModulePath(const std::string& path) { modulePath = path; }
    
    // 缓存管理
    void cacheCompiledResult(const std::string& key, std::shared_ptr<ASTNode> ast);
    std::shared_ptr<ASTNode> getCachedResult(const std::string& key) const;
    void clearCache() { compiledCache.clear(); }
    
    // 统计信息
    void incrementNodeCount(const std::string& nodeType);
    std::unordered_map<std::string, size_t> getStatistics() const { return statistics; }
    
    // 上下文重置
    void reset();
    
    // 调试支持
    void enableDebugMode(bool enable) { options.enableDebug = enable; }
    bool isDebugMode() const { return options.enableDebug; }
    void debugPrint(const std::string& message) const;
    
private:
    // 编译选项
    CompileOptions options;
    
    // 文件信息
    std::string currentFile;
    std::vector<std::string> sourceFiles;
    std::string modulePath;  // 官方模块路径
    
    // 核心组件
    std::unique_ptr<CHTLGlobalMap> globalMap;
    std::unique_ptr<CHTLStateManager> stateManager;
    std::unique_ptr<ConstraintManager> constraintManager;
    
    // 配置
    std::shared_ptr<ConfigurationNode> activeConfig;
    
    // 错误和警告
    std::vector<CompileError> errors;
    std::vector<CompileError> warnings;
    
    // 作用域栈
    std::vector<std::string> scopeStack;
    
    // 元素路径栈（用于自动化）
    std::vector<std::string> elementStack;
    
    // 编译缓存
    std::unordered_map<std::string, std::shared_ptr<ASTNode>> compiledCache;
    
    // 统计信息
    std::unordered_map<std::string, size_t> statistics;
    
    // 辅助方法
    std::string buildElementPath() const;
    std::string searchModulePath(const std::string& name, const std::string& type) const;
};

// 上下文管理器（单例模式）
class ContextManager {
public:
    static CHTLContext& getInstance();
    static void reset();
    static void setContext(std::unique_ptr<CHTLContext> ctx);
    
private:
    ContextManager() = default;
    static std::unique_ptr<CHTLContext> instance;
};

} // namespace chtl

#endif // CHTL_CONTEXT_H