#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>

namespace CHTL {

// 前向声明
class CHTLTemplate;
class CHTLCustom;
class CHTLConfiguration;
class CHTLNamespace;
class CHTLImport;

/**
 * 模板类型枚举
 */
enum class TemplateType {
    STYLE,      // @Style
    ELEMENT,    // @Element
    VAR         // @Var
};

/**
 * 自定义类型枚举
 */
enum class CustomType {
    STYLE,      // @Style
    ELEMENT,    // @Element
    VAR         // @Var
};

/**
 * 原始嵌入类型枚举
 */
enum class OriginType {
    HTML,           // @Html
    STYLE,          // @Style
    JAVASCRIPT,     // @JavaScript
    CUSTOM          // 自定义类型
};

/**
 * 导入类型枚举
 */
enum class ImportType {
    HTML,           // @Html
    STYLE,          // @Style
    JAVASCRIPT,     // @JavaScript
    CHTL,           // @Chtl
    CJMOD,          // @CJmod
    CONFIG           // @Config
};

/**
 * 模板基类
 */
class CHTLTemplate {
public:
    TemplateType type;
    std::string name;
    std::string content;
    std::vector<std::string> dependencies;
    bool isInherited;
    std::string parentName;
    
    CHTLTemplate(TemplateType t, const std::string& n)
        : type(t), name(n), isInherited(false) {}
    
    virtual ~CHTLTemplate() = default;
};

/**
 * 自定义基类
 */
class CHTLCustom {
public:
    CustomType type;
    std::string name;
    std::string content;
    std::vector<std::string> dependencies;
    bool isInherited;
    std::string parentName;
    
    CHTLCustom(CustomType t, const std::string& n)
        : type(t), name(n), isInherited(false) {}
    
    virtual ~CHTLCustom() = default;
};

/**
 * 配置组
 */
class CHTLConfiguration {
public:
    std::string name;                           // 配置组名称（可选）
    std::unordered_map<std::string, std::string> settings;  // 配置键值对
    std::vector<std::string> originTypes;       // 原始嵌入类型
    std::vector<std::string> keywords;          // 关键字配置
    
    CHTLConfiguration() = default;
    explicit CHTLConfiguration(const std::string& n) : name(n) {}
};

/**
 * 命名空间
 */
class CHTLNamespace {
public:
    std::string name;
    std::string fullPath;
    std::vector<std::string> children;
    std::string parent;
    bool isDefault;  // 是否为默认命名空间（文件名）
    
    CHTLNamespace(const std::string& n) : name(n), isDefault(false) {}
};

/**
 * 导入信息
 */
class CHTLImport {
public:
    ImportType type;
    std::string path;
    std::string alias;
    std::string targetName;
    bool hasAlias;
    
    CHTLImport(ImportType t, const std::string& p)
        : type(t), path(p), hasAlias(false) {}
};

/**
 * CHTL全局状态表
 * 管理所有全局状态，包括模板、自定义、配置、命名空间等
 */
class CHTLGlobalMap {
private:
    // 模板管理
    std::unordered_map<std::string, std::shared_ptr<CHTLTemplate>> styleTemplates;
    std::unordered_map<std::string, std::shared_ptr<CHTLTemplate>> elementTemplates;
    std::unordered_map<std::string, std::shared_ptr<CHTLTemplate>> varTemplates;
    
    // 自定义管理
    std::unordered_map<std::string, std::shared_ptr<CHTLCustom>> styleCustoms;
    std::unordered_map<std::string, std::shared_ptr<CHTLCustom>> elementCustoms;
    std::unordered_map<std::string, std::shared_ptr<CHTLCustom>> varCustoms;
    
    // 配置管理
    std::vector<std::shared_ptr<CHTLConfiguration>> configurations;
    std::shared_ptr<CHTLConfiguration> activeConfiguration;
    
    // 命名空间管理
    std::unordered_map<std::string, std::shared_ptr<CHTLNamespace>> namespaces;
    std::string currentNamespace;
    
    // 导入管理
    std::vector<std::shared_ptr<CHTLImport>> imports;
    
    // 文件信息
    std::string currentFileName;
    std::string currentFilePath;
    
    // 模块搜索路径
    std::vector<std::string> moduleSearchPaths;
    std::string officialModulePath;

public:
    CHTLGlobalMap();
    ~CHTLGlobalMap() = default;
    
    // 模板管理方法
    void addStyleTemplate(const std::string& name, std::shared_ptr<CHTLTemplate> template_);
    void addElementTemplate(const std::string& name, std::shared_ptr<CHTLTemplate> template_);
    void addVarTemplate(const std::string& name, std::shared_ptr<CHTLTemplate> template_);
    
    std::shared_ptr<CHTLTemplate> getStyleTemplate(const std::string& name);
    std::shared_ptr<CHTLTemplate> getElementTemplate(const std::string& name);
    std::shared_ptr<CHTLTemplate> getVarTemplate(const std::string& name);
    
    bool hasStyleTemplate(const std::string& name) const;
    bool hasElementTemplate(const std::string& name) const;
    bool hasVarTemplate(const std::string& name) const;
    
    // 自定义管理方法
    void addStyleCustom(const std::string& name, std::shared_ptr<CHTLCustom> custom);
    void addElementCustom(const std::string& name, std::shared_ptr<CHTLCustom> custom);
    void addVarCustom(const std::string& name, std::shared_ptr<CHTLCustom> custom);
    
    std::shared_ptr<CHTLCustom> getStyleCustom(const std::string& name);
    std::shared_ptr<CHTLCustom> getElementCustom(const std::string& name);
    std::shared_ptr<CHTLCustom> getVarCustom(const std::string& name);
    
    bool hasStyleCustom(const std::string& name) const;
    bool hasElementCustom(const std::string& name) const;
    bool hasVarCustom(const std::string& name) const;
    
    // 配置管理方法
    void addConfiguration(std::shared_ptr<CHTLConfiguration> config);
    void setActiveConfiguration(const std::string& name);
    std::shared_ptr<CHTLConfiguration> getActiveConfiguration() const;
    std::shared_ptr<CHTLConfiguration> getConfiguration(const std::string& name) const;
    
    // 命名空间管理方法
    void addNamespace(const std::string& name, std::shared_ptr<CHTLNamespace> ns);
    void setCurrentNamespace(const std::string& name);
    std::string getCurrentNamespace() const;
    std::shared_ptr<CHTLNamespace> getNamespace(const std::string& name) const;
    
    // 导入管理方法
    void addImport(std::shared_ptr<CHTLImport> import);
    std::vector<std::shared_ptr<CHTLImport>> getImports() const;
    
    // 文件管理方法
    void setCurrentFile(const std::string& fileName, const std::string& filePath);
    std::string getCurrentFileName() const;
    std::string getCurrentFilePath() const;
    
    // 模块搜索路径管理
    void addModuleSearchPath(const std::string& path);
    void setOfficialModulePath(const std::string& path);
    std::vector<std::string> getModuleSearchPaths() const;
    std::string getOfficialModulePath() const;
    
    // 冲突检测
    bool hasNameConflict(const std::string& name, TemplateType type) const;
    bool hasNameConflict(const std::string& name, CustomType type) const;
    bool hasNamespaceConflict(const std::string& name) const;
    
    // 清理方法
    void clear();
    void clearTemplates();
    void clearCustoms();
    void clearConfigurations();
    void clearNamespaces();
    void clearImports();
    
    // 调试方法
    void printStatus() const;
    size_t getTemplateCount() const;
    size_t getCustomCount() const;
    size_t getConfigurationCount() const;
    size_t getNamespaceCount() const;
    size_t getImportCount() const;
};

} // namespace CHTL