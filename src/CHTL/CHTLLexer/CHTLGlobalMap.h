#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "CHTLToken.h"

namespace CHTL {

// CHTL全局映射表 - 管理CHTL语法元素的映射和查找
class CHTLGlobalMap {
public:
    static CHTLGlobalMap& Instance();
    
    // 初始化默认映射
    void Initialize();
    
    // HTML元素检查
    bool IsHTMLElement(const std::string& name) const;
    
    // 关键字映射
    bool IsKeyword(const std::string& keyword) const;
    CHTLTokenType GetKeywordType(const std::string& keyword) const;
    
    // 模板和自定义类型映射  
    bool IsTemplateType(const std::string& type) const;
    bool IsCustomType(const std::string& type) const;
    bool IsOriginType(const std::string& type) const;
    
    // 配置相关
    void UpdateConfiguration(const std::unordered_map<std::string, std::string>& config);
    std::string GetConfigValue(const std::string& key) const;
    
    // 自定义原始嵌入类型管理
    void RegisterOriginType(const std::string& typeName);
    bool IsRegisteredOriginType(const std::string& typeName) const;
    
    // 命名空间管理
    void RegisterNamespace(const std::string& namespaceName);
    bool IsNamespaceRegistered(const std::string& namespaceName) const;
    void EnterNamespace(const std::string& namespaceName);
    void ExitNamespace();
    std::string GetCurrentNamespace() const;
    
    // 模板和自定义元素注册
    void RegisterTemplate(const std::string& type, const std::string& name);
    void RegisterCustom(const std::string& type, const std::string& name);
    bool IsTemplateRegistered(const std::string& type, const std::string& name) const;
    bool IsCustomRegistered(const std::string& type, const std::string& name) const;
    
    // 清理
    void Clear();
    
private:
    CHTLGlobalMap() = default;
    ~CHTLGlobalMap() = default;
    CHTLGlobalMap(const CHTLGlobalMap&) = delete;
    CHTLGlobalMap& operator=(const CHTLGlobalMap&) = delete;
    
    // 内部数据结构
    std::unordered_set<std::string> htmlElements_;
    std::unordered_map<std::string, CHTLTokenType> keywords_;
    std::unordered_set<std::string> templateTypes_;
    std::unordered_set<std::string> customTypes_;
    std::unordered_set<std::string> originTypes_;
    std::unordered_set<std::string> registeredOriginTypes_;
    std::unordered_set<std::string> namespaces_;
    std::string currentNamespace_;
    
    // 配置
    std::unordered_map<std::string, std::string> configuration_;
    
    // 注册的模板和自定义元素
    std::unordered_map<std::string, std::unordered_set<std::string>> registeredTemplates_;
    std::unordered_map<std::string, std::unordered_set<std::string>> registeredCustoms_;
    
    // 初始化方法
    void InitializeHTMLElements();
    void InitializeKeywords();
    void InitializeDefaultConfiguration();
};

} // namespace CHTL