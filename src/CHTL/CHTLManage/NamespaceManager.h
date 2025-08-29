#ifndef CHTL_NAMESPACE_MANAGER_H
#define CHTL_NAMESPACE_MANAGER_H

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <optional>

namespace CHTL {

// 命名空间类型
enum class NamespaceType {
    EXPLICIT,    // 显式声明的命名空间
    DEFAULT,     // 默认命名空间（文件名）
    MERGED       // 合并的命名空间
};

// 命名空间中的符号
struct NamespaceSymbol {
    std::string name;
    std::string type;        // "template", "custom", "origin", etc.
    std::string sourceFile;
    size_t line;
    size_t column;
};

// 命名空间信息
class NamespaceInfo {
public:
    NamespaceInfo(const std::string& name, NamespaceType type, const std::string& sourceFile)
        : name_(name), type_(type), primarySourceFile_(sourceFile) {}
    
    const std::string& getName() const { return name_; }
    NamespaceType getType() const { return type_; }
    const std::string& getPrimarySourceFile() const { return primarySourceFile_; }
    
    // 符号管理
    void addSymbol(const NamespaceSymbol& symbol);
    bool hasSymbol(const std::string& name) const;
    std::optional<NamespaceSymbol> getSymbol(const std::string& name) const;
    std::vector<NamespaceSymbol> getAllSymbols() const;
    
    // 子命名空间管理
    void addChildNamespace(const std::string& child);
    bool hasChildNamespace(const std::string& child) const;
    const std::unordered_set<std::string>& getChildNamespaces() const { return childNamespaces_; }
    
    // 约束管理
    void addConstraint(const std::string& constraint);
    bool isConstraintViolated(const std::string& item) const;
    
    // 合并来源追踪
    void addMergeSource(const std::string& sourceFile);
    const std::vector<std::string>& getMergeSources() const { return mergeSources_; }

private:
    std::string name_;
    NamespaceType type_;
    std::string primarySourceFile_;
    std::unordered_map<std::string, NamespaceSymbol> symbols_;
    std::unordered_set<std::string> childNamespaces_;
    std::unordered_set<std::string> constraints_;
    std::vector<std::string> mergeSources_;
};

// 命名空间管理器
class NamespaceManager {
public:
    static NamespaceManager& getInstance() {
        static NamespaceManager instance;
        return instance;
    }
    
    // 注册命名空间
    void registerNamespace(const std::string& name, NamespaceType type, 
                          const std::string& sourceFile);
    
    // 获取命名空间
    std::shared_ptr<NamespaceInfo> getNamespace(const std::string& name) const;
    
    // 合并同名命名空间
    void mergeNamespaces(const std::string& name);
    
    // 添加符号到命名空间
    void addSymbolToNamespace(const std::string& namespaceName, 
                             const NamespaceSymbol& symbol);
    
    // 解析符号（支持嵌套命名空间）
    std::optional<NamespaceSymbol> resolveSymbol(const std::string& symbolPath) const;
    
    // 检查命名空间冲突
    struct ConflictInfo {
        std::string namespaceName;
        std::string symbolName;
        std::vector<std::string> conflictingFiles;
        std::string reason;
    };
    
    std::vector<ConflictInfo> checkConflicts() const;
    
    // 获取文件的默认命名空间
    std::string getDefaultNamespace(const std::string& filePath) const;
    
    // 设置文件的默认命名空间
    void setDefaultNamespace(const std::string& filePath, const std::string& namespaceName);
    
    // 检查约束违反
    bool checkConstraintViolation(const std::string& namespaceName, 
                                 const std::string& item) const;
    
    // 获取所有命名空间
    std::vector<std::string> getAllNamespaces() const;
    
    // 清空管理器
    void clear();

private:
    NamespaceManager() = default;
    NamespaceManager(const NamespaceManager&) = delete;
    NamespaceManager& operator=(const NamespaceManager&) = delete;
    
    // 命名空间映射
    std::unordered_map<std::string, std::shared_ptr<NamespaceInfo>> namespaces_;
    
    // 文件到默认命名空间的映射
    std::unordered_map<std::string, std::string> fileToNamespace_;
    
    // 辅助方法
    std::pair<std::string, std::string> splitSymbolPath(const std::string& path) const;
    bool isValidNamespaceName(const std::string& name) const;
    std::string extractFilenameAsNamespace(const std::string& filePath) const;
};

// 命名空间解析器
class NamespaceResolver {
public:
    explicit NamespaceResolver(NamespaceManager& manager) : manager_(manager) {}
    
    // 解析完整的命名空间路径
    std::shared_ptr<NamespaceInfo> resolve(const std::string& path) const;
    
    // 解析符号（支持from语法）
    std::optional<NamespaceSymbol> resolveSymbol(const std::string& symbol, 
                                                 const std::string& fromNamespace) const;
    
    // 构建完整路径
    std::string buildFullPath(const std::string& namespacePath, 
                             const std::string& symbol) const;

private:
    NamespaceManager& manager_;
};

} // namespace CHTL

#endif // CHTL_NAMESPACE_MANAGER_H