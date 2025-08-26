#ifndef CHTL_DEPENDENCY_MANAGER_H
#define CHTL_DEPENDENCY_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace CHTL {

// 依赖类型
enum class DependencyType {
    FILE_IMPORT,        // 文件导入依赖
    MODULE_IMPORT,      // 模块导入依赖
    SYMBOL_REFERENCE,   // 符号引用依赖
    NAMESPACE_USE,      // 命名空间使用依赖
    CONFIG_REFERENCE    // 配置引用依赖
};

// 依赖信息
struct DependencyInfo {
    std::string source;         // 依赖源
    std::string target;         // 依赖目标
    DependencyType type;        // 依赖类型
    size_t line;               // 依赖发生的行号
    size_t column;             // 依赖发生的列号
    bool isResolved;           // 是否已解析
    bool isOptional;           // 是否为可选依赖
    
    DependencyInfo() : type(DependencyType::FILE_IMPORT), 
                      line(0), column(0), 
                      isResolved(false), isOptional(false) {}
};

// 循环依赖信息
struct CircularDependency {
    std::vector<std::string> cycle;  // 形成循环的路径
    DependencyType type;             // 循环依赖的类型
    std::string description;         // 描述信息
};

// 依赖管理器 - 负责依赖关系的跟踪和分析
class DependencyManager {
private:
    // 依赖图：源 -> 目标列表
    std::unordered_map<std::string, std::unordered_set<std::string>> dependencyGraph;
    
    // 反向依赖图：目标 -> 源列表
    std::unordered_map<std::string, std::unordered_set<std::string>> reverseDependencyGraph;
    
    // 详细依赖信息
    std::unordered_map<std::pair<std::string, std::string>, DependencyInfo, 
                      std::hash<std::pair<std::string, std::string>>> dependencyDetails;
    
    // 循环依赖检测状态
    mutable std::unordered_set<std::string> visitStack;
    mutable std::unordered_set<std::string> visited;
    mutable std::vector<CircularDependency> detectedCycles;
    
    // 辅助方法
    bool detectCycleUtil(const std::string& node, std::vector<std::string>& path) const;
    void buildTransitiveClosure();
    
public:
    DependencyManager();
    ~DependencyManager() = default;
    
    // 依赖添加
    void addDependency(const DependencyInfo& dependency);
    void addDependency(const std::string& source, const std::string& target, DependencyType type);
    void addFileDependency(const std::string& source, const std::string& target);
    void addModuleDependency(const std::string& source, const std::string& target);
    void addSymbolDependency(const std::string& source, const std::string& target);
    
    // 依赖移除
    void removeDependency(const std::string& source, const std::string& target);
    void removeAllDependenciesFrom(const std::string& source);
    void removeAllDependenciesTo(const std::string& target);
    
    // 依赖查询
    std::unordered_set<std::string> getDirectDependencies(const std::string& source) const;
    std::unordered_set<std::string> getDirectDependents(const std::string& target) const;
    std::unordered_set<std::string> getAllDependencies(const std::string& source) const;
    std::unordered_set<std::string> getAllDependents(const std::string& target) const;
    
    // 依赖信息查询
    DependencyInfo getDependencyInfo(const std::string& source, const std::string& target) const;
    std::vector<DependencyInfo> getAllDependencyInfo(const std::string& source) const;
    bool hasDependency(const std::string& source, const std::string& target) const;
    bool hasDependencyOfType(const std::string& source, const std::string& target, DependencyType type) const;
    
    // 循环依赖检测
    bool hasCircularDependency() const;
    bool hasCircularDependency(const std::string& source, const std::string& target) const;
    std::vector<CircularDependency> detectAllCircularDependencies() const;
    std::vector<std::string> findCircularPath(const std::string& start) const;
    
    // 依赖顺序
    std::vector<std::string> getTopologicalOrder() const;
    std::vector<std::string> getDependencyOrder(const std::string& root) const;
    bool canReach(const std::string& source, const std::string& target) const;
    
    // 依赖分析
    size_t getDependencyCount(const std::string& node) const;
    size_t getDependentCount(const std::string& node) const;
    std::vector<std::string> getIndependentNodes() const;
    std::vector<std::string> getLeafNodes() const;
    
    // 批量操作
    void markDependenciesResolved(const std::string& source);
    void markAllDependenciesResolved();
    std::vector<std::string> getUnresolvedDependencies() const;
    
    // 清理
    void clear();
    void clearNode(const std::string& node);
    
    // 验证
    bool validateDependencyGraph() const;
    std::vector<std::string> getDependencyErrors() const;
    
    // 调试
    void dumpDependencyGraph() const;
    void dumpCircularDependencies() const;
    void dumpDependencyStats() const;
};

} // namespace CHTL

#endif // CHTL_DEPENDENCY_MANAGER_H