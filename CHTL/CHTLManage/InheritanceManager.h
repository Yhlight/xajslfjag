#ifndef CHTL_INHERITANCE_MANAGER_H
#define CHTL_INHERITANCE_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace CHTL {

// 继承类型
enum class InheritanceType {
    STYLE_GROUP,        // 样式组继承
    ELEMENT,            // 元素继承
    VARIABLE_GROUP,     // 变量组继承
    EXPLICIT,           // 显式继承 (inherit关键字)
    IMPLICIT            // 隐式继承 (组合式)
};

// 继承节点
struct InheritanceNode {
    std::string name;
    std::string type;           // @Style, @Element, @Var
    InheritanceType inheritType;
    std::vector<std::string> parents;
    std::vector<std::string> children;
    std::unordered_set<std::string> deletedProperties;
    std::unordered_set<std::string> deletedInheritances;
    
    InheritanceNode() : inheritType(InheritanceType::IMPLICIT) {}
};

// 属性信息
struct PropertyInfo {
    std::string name;
    std::string value;
    std::string source;         // 属性来源（哪个父类）
    int priority;               // 优先级（用于解决冲突）
    bool isDeleted;             // 是否被删除
    
    PropertyInfo() : priority(0), isDeleted(false) {}
};

// 继承解析结果
struct InheritanceResolution {
    std::unordered_map<std::string, PropertyInfo> properties;
    std::vector<std::string> inheritanceChain;
    std::vector<std::string> conflicts;
    bool hasCircularInheritance;
    
    InheritanceResolution() : hasCircularInheritance(false) {}
};

// 继承管理器 - 负责继承关系的管理和解析
class InheritanceManager {
private:
    // 继承图：节点名 -> 继承节点
    std::unordered_map<std::string, std::shared_ptr<InheritanceNode>> inheritanceGraph;
    
    // 属性表：节点名 -> 属性映射
    std::unordered_map<std::string, std::unordered_map<std::string, PropertyInfo>> propertyTable;
    
    // 循环继承检测
    mutable std::unordered_set<std::string> visitStack;
    mutable std::unordered_set<std::string> visited;
    
    // 辅助方法
    bool detectCircularInheritance(const std::string& node) const;
    void collectInheritedProperties(
        const std::string& node, 
        InheritanceResolution& result,
        std::unordered_set<std::string>& visitedNodes,
        int depth
    ) const;
    void resolvePropertyConflicts(InheritanceResolution& result) const;
    
public:
    InheritanceManager();
    ~InheritanceManager() = default;
    
    // 节点管理
    void createNode(const std::string& name, const std::string& type);
    void removeNode(const std::string& name);
    bool nodeExists(const std::string& name) const;
    std::shared_ptr<InheritanceNode> getNode(const std::string& name) const;
    
    // 继承关系管理
    void addInheritance(const std::string& child, const std::string& parent, InheritanceType type);
    void removeInheritance(const std::string& child, const std::string& parent);
    void clearInheritances(const std::string& node);
    std::vector<std::string> getParents(const std::string& node) const;
    std::vector<std::string> getChildren(const std::string& node) const;
    
    // 属性管理
    void addProperty(const std::string& node, const std::string& property, const std::string& value);
    void removeProperty(const std::string& node, const std::string& property);
    void deleteProperty(const std::string& node, const std::string& property);
    void deleteInheritance(const std::string& node, const std::string& parent);
    PropertyInfo getProperty(const std::string& node, const std::string& property) const;
    std::unordered_map<std::string, PropertyInfo> getProperties(const std::string& node) const;
    
    // 继承解析
    InheritanceResolution resolveInheritance(const std::string& node) const;
    std::unordered_map<std::string, PropertyInfo> getEffectiveProperties(const std::string& node) const;
    std::vector<std::string> getInheritanceChain(const std::string& node) const;
    bool hasProperty(const std::string& node, const std::string& property, bool includeInherited = true) const;
    
    // 循环继承检测
    bool hasCircularInheritance(const std::string& node) const;
    std::vector<std::vector<std::string>> detectAllCircularInheritances() const;
    
    // 继承树操作
    std::vector<std::string> getAncestors(const std::string& node) const;
    std::vector<std::string> getDescendants(const std::string& node) const;
    int getInheritanceDepth(const std::string& node) const;
    bool isAncestor(const std::string& node, const std::string& potentialAncestor) const;
    
    // 批量操作
    void mergeNodes(const std::string& target, const std::string& source);
    void copyInheritance(const std::string& source, const std::string& target);
    
    // 验证
    bool validateInheritanceGraph() const;
    std::vector<std::string> getInheritanceErrors() const;
    
    // 清理
    void clear();
    void clearNode(const std::string& node);
    
    // 调试
    void dumpInheritanceGraph() const;
    void dumpPropertyTable() const;
    void dumpInheritanceTree(const std::string& root = "") const;
};

} // namespace CHTL

#endif // CHTL_INHERITANCE_MANAGER_H