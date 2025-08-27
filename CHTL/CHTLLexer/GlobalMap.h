#pragma once
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <vector>

namespace CHTL {

// 前向声明
class TemplateNode;
class CustomNode;
class NamespaceNode;
class ConfigNode;

/**
 * CHTL全局映射管理器
 * 负责管理模板、自定义、命名空间、配置等全局资源
 */
class GlobalMap {
public:
    GlobalMap() = default;
    ~GlobalMap() = default;

    // 模板管理
    void registerTemplate(const std::string& type, const std::string& name, 
                         std::shared_ptr<TemplateNode> node);
    std::shared_ptr<TemplateNode> getTemplate(const std::string& type, 
                                             const std::string& name) const;
    bool hasTemplate(const std::string& type, const std::string& name) const;
    void removeTemplate(const std::string& type, const std::string& name);

    // 自定义管理
    void registerCustom(const std::string& type, const std::string& name, 
                       std::shared_ptr<CustomNode> node);
    std::shared_ptr<CustomNode> getCustom(const std::string& type, 
                                         const std::string& name) const;
    bool hasCustom(const std::string& type, const std::string& name) const;
    void removeCustom(const std::string& type, const std::string& name);

    // 命名空间管理
    void registerNamespace(const std::string& name, 
                          std::shared_ptr<NamespaceNode> node);
    std::shared_ptr<NamespaceNode> getNamespace(const std::string& name) const;
    bool hasNamespace(const std::string& name) const;
    void removeNamespace(const std::string& name);

    // 配置管理
    void registerConfig(const std::string& name, 
                       std::shared_ptr<ConfigNode> node);
    std::shared_ptr<ConfigNode> getConfig(const std::string& name) const;
    std::shared_ptr<ConfigNode> getActiveConfig() const;
    void setActiveConfig(const std::string& name);
    bool hasConfig(const std::string& name) const;

    // 原始嵌入管理
    void registerOrigin(const std::string& type, const std::string& name, 
                       const std::string& content);
    std::string getOrigin(const std::string& type, const std::string& name) const;
    bool hasOrigin(const std::string& type, const std::string& name) const;

    // 导入路径管理
    void addImportPath(const std::string& path);
    void removeImportPath(const std::string& path);
    std::vector<std::string> getImportPaths() const;
    bool isPathImported(const std::string& path) const;

    // 循环依赖检测
    void pushImportStack(const std::string& path);
    void popImportStack();
    bool hasCircularDependency(const std::string& path) const;

    // HTML标签类名/ID自动分配
    std::string allocateClassName(const std::string& base = "");
    std::string allocateIdName(const std::string& base = "");
    bool isClassNameUsed(const std::string& name) const;
    bool isIdNameUsed(const std::string& name) const;

    // 类/ID选择器管理
    void registerClassSelector(const std::string& name);
    void registerIdSelector(const std::string& name);
    std::unordered_set<std::string> getClassSelectors() const;
    std::unordered_set<std::string> getIdSelectors() const;

    // 清理方法
    void clear();
    void clearTemplates();
    void clearCustoms();
    void clearNamespaces();
    void clearConfigs();

private:
    // 模板存储 key: "type:name"
    std::unordered_map<std::string, std::shared_ptr<TemplateNode>> templates_;
    
    // 自定义存储 key: "type:name"
    std::unordered_map<std::string, std::shared_ptr<CustomNode>> customs_;
    
    // 命名空间存储
    std::unordered_map<std::string, std::shared_ptr<NamespaceNode>> namespaces_;
    
    // 配置存储
    std::unordered_map<std::string, std::shared_ptr<ConfigNode>> configs_;
    std::string activeConfigName_;
    
    // 原始嵌入存储 key: "type:name"
    std::unordered_map<std::string, std::string> origins_;
    
    // 导入路径管理
    std::unordered_set<std::string> importedPaths_;
    std::vector<std::string> importStack_;
    
    // 类名/ID管理
    std::unordered_set<std::string> usedClassNames_;
    std::unordered_set<std::string> usedIdNames_;
    std::unordered_set<std::string> classSelectors_;
    std::unordered_set<std::string> idSelectors_;
    
    size_t classCounter_ = 0;
    size_t idCounter_ = 0;

    // 辅助方法
    std::string makeKey(const std::string& type, const std::string& name) const;
};

} // namespace CHTL