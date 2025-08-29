#pragma once

#include "CHTLJSNodes.h"
#include <queue>
#include <unordered_set>

namespace CHTL {

// 模块入口结构
struct ModuleEntry {
    String path;                    // 原始路径
    String resolvedPath;            // 解析后的完整路径
    String type;                    // cjjs, js, auto
    String name;                    // 模块名称
    bool isLoaded;                  // 是否已加载
    StringVector dependencies;      // 依赖的其他模块
    StringUnorderedMap metadata;    // 模块元数据
    
    ModuleEntry(const String& p = "", const String& t = "auto") 
        : path(p), type(t), isLoaded(false) {}
};

// 模块加载策略
enum class ModuleLoadStrategy {
    SEQUENTIAL,     // 顺序加载
    PARALLEL,       // 并行加载
    DEPENDENCY      // 按依赖关系加载
};

// CHTL JS模块节点
class ModuleNode : public CHTLJSNode {
public:
    std::vector<ModuleEntry> modules;
    ModuleLoadStrategy loadStrategy;
    bool enableCircularCheck;
    bool enableAsyncLoading;
    int loadTimeout;  // 加载超时时间(ms)
    String basePath;  // 基础路径
    
    // 兼容性支持字段
    String moduleContent;  // 模块内容
    String modulePath;     // 模块路径
    String loadStrategyStr; // 加载策略字符串
    StringVector dependencies; // 依赖列表
    StringVector exports;      // 导出列表
    
    explicit ModuleNode(const String& name = "", const Position& pos = Position())
        : CHTLJSNode(NodeType::CHTLJS_MODULE, name, pos),
          loadStrategy(ModuleLoadStrategy::DEPENDENCY),
          enableCircularCheck(true),
          enableAsyncLoading(true),
          loadTimeout(5000),
          basePath("./") {}
    
    // 模块管理
    void addModule(const String& path, const String& type = "auto");
    void addModuleWithMetadata(const String& path, const String& type, const StringUnorderedMap& metadata);
    void removeModule(const String& path);
    void clearModules();
    
    // 模块信息
    std::vector<ModuleEntry> getModules() const { return modules; }
    size_t getModuleCount() const { return modules.size(); }
    bool hasModule(const String& path) const;
    ModuleEntry* findModule(const String& path);
    const ModuleEntry* findModule(const String& path) const;
    
    // 路径解析
    String resolveModulePath(const String& rawPath) const;
    String resolveRelativePath(const String& relativePath) const;
    String searchInModuleDirectories(const String& moduleName) const;
    String detectModuleType(const String& filePath) const;
    
    // 依赖分析
    void analyzeDependencies();
    StringVector getLoadOrder();
    bool hasCircularDependency() const;
    StringVector findCircularDependencyPath() const;
    void validateDependencies();
    
    // 配置
    void setLoadStrategy(ModuleLoadStrategy strategy) { loadStrategy = strategy; }
    ModuleLoadStrategy getLoadStrategy() const { return loadStrategy; }
    void setCircularCheck(bool enable) { enableCircularCheck = enable; }
    bool getCircularCheck() const { return enableCircularCheck; }
    void setAsyncLoading(bool enable) { enableAsyncLoading = enable; }
    bool getAsyncLoading() const { return enableAsyncLoading; }
    void setLoadTimeout(int timeout) { loadTimeout = timeout; }
    int getLoadTimeout() const { return loadTimeout; }
    void setBasePath(const String& path) { basePath = path; }
    String getBasePath() const { return basePath; }
    
    // 兼容性方法（为了向后兼容生成器接口）
    StringVector getDependencies() const;
    StringVector getExports() const;
    String getContent() const;
    String getPath() const;
    String getLoadStrategy() const;
    void setContent(const String& content);
    void setPath(const String& path);
    void setLoadStrategy(const String& strategy);
    void addDependency(const String& dependency);
    void addExport(const String& exportName);
    String getModuleName() const;
    
    // JavaScript生成
    String generateJavaScript() const override;
    String generateAMDLoader() const;
    String generateModuleRegistry() const;
    String generateLoadSequence() const;
    String generateErrorHandling() const;
    String generateModuleWrapper(const ModuleEntry& module) const;
    
    // 验证
    bool validateCHTLJSSyntax() const override;
    StringVector getCHTLJSValidationErrors() const override;
    bool validateModulePaths() const;
    StringVector getPathValidationErrors() const;
    
    // 重写基类方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
    
private:
    // 依赖分析辅助方法
    void buildDependencyGraph(std::unordered_map<String, StringVector>& graph) const;
    StringVector topologicalSort() const;
    bool hasCycleDFS(const String& node, 
                     const std::unordered_map<String, StringVector>& graph,
                     std::unordered_set<String>& visited,
                     std::unordered_set<String>& recStack) const;
    
    // 路径解析辅助方法
    StringVector getSearchPaths() const;
    StringVector getSupportedExtensions() const;
    bool isValidModulePath(const String& path) const;
    
    // JavaScript生成辅助方法
    String generateDefineFunction() const;
    String generateLoadFunction() const;
    String generateDependencyLoader() const;
    String generateModuleExecutor() const;
    String generateProgressCallback() const;
    String generateErrorCallback() const;
};

// 模块加载器类
class ModuleLoader {
public:
    struct LoadResult {
        bool success;
        String content;
        String resolvedPath;
        String type;
        StringVector errors;
        StringVector dependencies;
    };
    
    static LoadResult loadModule(const String& path, const String& basePath = "./");
    static String extractDependencies(const String& content, const String& type);
    static bool validateModuleContent(const String& content, const String& type);
    static String resolveModuleType(const String& filePath);
    
private:
    static String readFileContent(const String& filePath);
    static StringVector extractCJJSDependencies(const String& content);
    static StringVector extractJSDependencies(const String& content);
};

} // namespace CHTL