#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * CHTL全局映射管理器
 * 负责管理所有模板、自定义、配置等全局信息
 * 严格按照CHTL语法文档实现
 */
class GlobalMap {
public:
    // 配置信息
    struct Configuration {
        int INDEX_INITIAL_COUNT = 0;
        bool DISABLE_NAME_GROUP = true;
        bool DISABLE_CUSTOM_ORIGIN_TYPE = false;
        bool DEBUG_MODE = false;
        bool DISABLE_STYLE_AUTO_ADD_CLASS = false;
        bool DISABLE_STYLE_AUTO_ADD_ID = false;
        bool DISABLE_SCRIPT_AUTO_ADD_CLASS = true;
        bool DISABLE_SCRIPT_AUTO_ADD_ID = true;
        bool DISABLE_DEFAULT_NAMESPACE = false;
        
        // Name配置块
        std::unordered_map<std::string, std::vector<std::string>> nameConfig;
        
        // 原始类型配置
        std::unordered_map<std::string, std::string> originTypes;
    };
    
    // 模板信息
    struct TemplateInfo {
        std::string name;
        std::string type;  // Style, Element, Var
        std::string content;
        std::string namespacePath;
        std::vector<std::string> inheritanceChain;
    };
    
    // 自定义信息
    struct CustomInfo {
        std::string name;
        std::string type;  // Style, Element, Var
        std::string content;
        std::string namespacePath;
        std::vector<std::string> specializations;
    };
    
    // 原始嵌入信息
    struct OriginInfo {
        std::string name;
        std::string type;
        std::string content;
        std::string namespacePath;
    };
    
    // 导入信息
    struct ImportInfo {
        std::string type;
        std::string path;
        std::string alias;
        std::string namespacePath;
        bool hasAlias;
    };
    
    // 命名空间信息
    struct NamespaceInfo {
        std::string name;
        std::string parentNamespace;
        std::vector<std::string> children;
        std::unordered_set<std::string> exports;
    };

private:
    Configuration globalConfig;
    std::unordered_map<std::string, TemplateInfo> styleTemplates;
    std::unordered_map<std::string, TemplateInfo> elementTemplates;
    std::unordered_map<std::string, TemplateInfo> varTemplates;
    
    std::unordered_map<std::string, CustomInfo> styleCustoms;
    std::unordered_map<std::string, CustomInfo> elementCustoms;
    std::unordered_map<std::string, CustomInfo> varCustoms;
    
    std::unordered_map<std::string, OriginInfo> origins;
    std::unordered_map<std::string, ImportInfo> imports;
    std::unordered_map<std::string, NamespaceInfo> namespaces;
    
    std::string currentNamespace;
    std::unordered_set<std::string> loadedFiles;
    
    static std::unique_ptr<GlobalMap> instance;

public:
    static GlobalMap& getInstance();
    static void reset();
    
    // 配置管理
    void setConfiguration(const Configuration& config);
    const Configuration& getConfiguration() const;
    void mergeConfiguration(const Configuration& config);
    
    // 模板管理
    void registerStyleTemplate(const std::string& name, const TemplateInfo& info);
    void registerElementTemplate(const std::string& name, const TemplateInfo& info);
    void registerVarTemplate(const std::string& name, const TemplateInfo& info);
    
    TemplateInfo* getStyleTemplate(const std::string& name);
    TemplateInfo* getElementTemplate(const std::string& name);
    TemplateInfo* getVarTemplate(const std::string& name);
    
    // 自定义管理
    void registerStyleCustom(const std::string& name, const CustomInfo& info);
    void registerElementCustom(const std::string& name, const CustomInfo& info);
    void registerVarCustom(const std::string& name, const CustomInfo& info);
    
    CustomInfo* getStyleCustom(const std::string& name);
    CustomInfo* getElementCustom(const std::string& name);
    CustomInfo* getVarCustom(const std::string& name);
    
    // 原始嵌入管理
    void registerOrigin(const std::string& name, const OriginInfo& info);
    OriginInfo* getOrigin(const std::string& name);
    
    // 导入管理
    void registerImport(const std::string& name, const ImportInfo& info);
    ImportInfo* getImport(const std::string& name);
    bool isFileLoaded(const std::string& filePath);
    void markFileLoaded(const std::string& filePath);
    
    // 命名空间管理
    void enterNamespace(const std::string& name);
    void exitNamespace();
    std::string getCurrentNamespace() const;
    void registerNamespace(const std::string& name, const NamespaceInfo& info);
    NamespaceInfo* getNamespace(const std::string& name);
    std::string resolveFullName(const std::string& name) const;
    
    // 查找功能 - 支持命名空间
    TemplateInfo* findTemplate(const std::string& type, const std::string& name);
    CustomInfo* findCustom(const std::string& type, const std::string& name);
    OriginInfo* findOrigin(const std::string& name);
    
    // 冲突检测
    bool hasConflict(const std::string& type, const std::string& name) const;
    std::vector<std::string> getConflicts(const std::string& type, const std::string& name) const;
    
    // 调试信息
    void dumpState() const;
    void clear();
};

}