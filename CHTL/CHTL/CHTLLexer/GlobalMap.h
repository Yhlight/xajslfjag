#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace CHTL {

// 前向声明
class TemplateNode;
class CustomNode;
class OriginNode;
class ConfigNode;
class NamespaceNode;

// 全局映射表，管理CHTL中的各种定义
class GlobalMap {
public:
    GlobalMap();
    ~GlobalMap();
    
    // 模板管理
    void AddTemplate(const std::string& type, const std::string& name, std::shared_ptr<TemplateNode> node);
    std::shared_ptr<TemplateNode> GetTemplate(const std::string& type, const std::string& name) const;
    bool HasTemplate(const std::string& type, const std::string& name) const;
    std::vector<std::string> GetTemplateNames(const std::string& type) const;
    
    // 自定义管理
    void AddCustom(const std::string& type, const std::string& name, std::shared_ptr<CustomNode> node);
    std::shared_ptr<CustomNode> GetCustom(const std::string& type, const std::string& name) const;
    bool HasCustom(const std::string& type, const std::string& name) const;
    std::vector<std::string> GetCustomNames(const std::string& type) const;
    
    // 原始嵌入管理
    void AddOrigin(const std::string& type, const std::string& name, std::shared_ptr<OriginNode> node);
    std::shared_ptr<OriginNode> GetOrigin(const std::string& type, const std::string& name) const;
    bool HasOrigin(const std::string& type, const std::string& name) const;
    std::vector<std::string> GetOriginNames(const std::string& type) const;
    
    // 配置管理
    void AddConfig(const std::string& name, std::shared_ptr<ConfigNode> node);
    std::shared_ptr<ConfigNode> GetConfig(const std::string& name) const;
    std::shared_ptr<ConfigNode> GetDefaultConfig() const;  // 获取无名配置
    bool HasConfig(const std::string& name) const;
    std::vector<std::string> GetConfigNames() const;
    
    // 命名空间管理
    void AddNamespace(const std::string& name, std::shared_ptr<NamespaceNode> node);
    std::shared_ptr<NamespaceNode> GetNamespace(const std::string& name) const;
    bool HasNamespace(const std::string& name) const;
    std::vector<std::string> GetNamespaceNames() const;
    void EnterNamespace(const std::string& name);
    void ExitNamespace();
    std::string GetCurrentNamespace() const;
    std::string GetFullName(const std::string& name) const;  // 获取带命名空间的完整名称
    
    // 导入管理
    void AddImportedFile(const std::string& path);
    bool IsFileImported(const std::string& path) const;
    std::vector<std::string> GetImportedFiles() const;
    
    // 自动生成的类名和ID管理
    std::string GenerateClassName();
    std::string GenerateId();
    void RegisterClassName(const std::string& className);
    void RegisterId(const std::string& id);
    bool IsClassNameUsed(const std::string& className) const;
    bool IsIdUsed(const std::string& id) const;
    
    // 清空所有映射
    void Clear();
    
    // 合并另一个GlobalMap
    void Merge(const GlobalMap& other);
    
private:
    // 模板存储 - key: type:name
    std::unordered_map<std::string, std::shared_ptr<TemplateNode>> templates;
    
    // 自定义存储 - key: type:name
    std::unordered_map<std::string, std::shared_ptr<CustomNode>> customs;
    
    // 原始嵌入存储 - key: type:name
    std::unordered_map<std::string, std::shared_ptr<OriginNode>> origins;
    
    // 配置存储 - key: name (空字符串表示默认配置)
    std::unordered_map<std::string, std::shared_ptr<ConfigNode>> configs;
    
    // 命名空间存储
    std::unordered_map<std::string, std::shared_ptr<NamespaceNode>> namespaces;
    std::vector<std::string> namespaceStack;  // 当前命名空间栈
    
    // 已导入的文件
    std::unordered_map<std::string, bool> importedFiles;
    
    // 已使用的类名和ID
    std::unordered_map<std::string, bool> usedClassNames;
    std::unordered_map<std::string, bool> usedIds;
    
    // 自动生成计数器
    int autoClassCounter;
    int autoIdCounter;
    
    // 辅助方法
    std::string MakeKey(const std::string& type, const std::string& name) const;
    std::pair<std::string, std::string> SplitKey(const std::string& key) const;
};

} // namespace CHTL