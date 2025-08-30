#pragma once
#include "BaseNode.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <filesystem>

namespace CHTL {

/**
 * 导入类型
 */
enum class ImportType {
    // 基础文件导入
    HTML_IMPORT,           // [Import] @Html
    STYLE_IMPORT,          // [Import] @Style  
    JAVASCRIPT_IMPORT,     // [Import] @JavaScript
    CHTL_IMPORT,          // [Import] @Chtl
    CJMOD_IMPORT,         // [Import] @CJmod
    CONFIG_IMPORT,        // [Import] @Config
    
    // 具体符号导入
    TEMPLATE_IMPORT,      // [Import] [Template] @Element/@Style/@Var
    CUSTOM_IMPORT,        // [Import] [Custom] @Element/@Style/@Var
    ORIGIN_IMPORT,        // [Import] [Origin] @Html/@Style/@JavaScript
    
    // 批量导入
    ALL_TEMPLATES_IMPORT, // [Import] [Template] from
    ALL_CUSTOMS_IMPORT,   // [Import] [Custom] from
    ALL_ORIGINS_IMPORT,   // [Import] [Origin] from
    ALL_CONFIGS_IMPORT    // [Import] [Configuration] from
};

/**
 * 导入路径类型
 */
enum class ImportPathType {
    FILENAME_ONLY,        // 文件名（不带后缀）
    FILENAME_WITH_EXT,    // 文件名（带后缀）
    FULL_PATH,           // 完整路径
    MODULE_NAME,         // 模块名称
    QUALIFIED_NAME       // 带命名空间的限定名
};

/**
 * 导入节点
 * 表示CHTL中的[Import]语句
 */
class ImportNode : public BaseNode {
public:
    /**
     * 构造函数
     */
    ImportNode(ImportType importType = ImportType::CHTL_IMPORT);
    
    /**
     * 析构函数
     */
    virtual ~ImportNode() = default;
    
    /**
     * 获取导入类型
     */
    ImportType getImportType() const;
    
    /**
     * 设置导入类型
     */
    void setImportType(ImportType type);
    
    /**
     * 获取导入路径
     */
    const std::string& getImportPath() const;
    
    /**
     * 设置导入路径
     */
    void setImportPath(const std::string& path);
    
    /**
     * 获取导入路径类型
     */
    ImportPathType getImportPathType() const;
    
    /**
     * 设置导入路径类型
     */
    void setImportPathType(ImportPathType pathType);
    
    /**
     * 获取导入的符号名称
     */
    const std::string& getSymbolName() const;
    
    /**
     * 设置导入的符号名称
     */
    void setSymbolName(const std::string& name);
    
    /**
     * 获取导入的符号类型（@Element, @Style, @Var等）
     */
    const std::string& getSymbolType() const;
    
    /**
     * 设置导入的符号类型
     */
    void setSymbolType(const std::string& type);
    
    /**
     * 获取别名（as后的名称）
     */
    const std::string& getAlias() const;
    
    /**
     * 设置别名
     */
    void setAlias(const std::string& alias);
    
    /**
     * 检查是否有别名
     */
    bool hasAlias() const;
    
    /**
     * 获取源文件命名空间
     */
    const std::string& getSourceNamespace() const;
    
    /**
     * 设置源文件命名空间
     */
    void setSourceNamespace(const std::string& namespaceName);
    
    /**
     * 获取导入的符号列表（用于批量导入）
     */
    const std::vector<std::string>& getImportedSymbols() const;
    
    /**
     * 添加导入的符号
     */
    void addImportedSymbol(const std::string& symbol);
    
    /**
     * 解析导入路径
     */
    std::string resolveImportPath() const;
    
    /**
     * 检查是否为模块导入
     */
    bool isModuleImport() const;
    
    /**
     * 检查是否为文件导入
     */
    bool isFileImport() const;
    
    /**
     * 检查是否为符号导入
     */
    bool isSymbolImport() const;
    
    /**
     * 检查是否为批量导入
     */
    bool isBatchImport() const;
    
    // BaseNode 接口实现
    CHTLNodeType getNodeType() const;
    std::string toString() const override;
    NodePtr clone() const override;
    bool validate(ErrorReporter* errorReporter = nullptr) const override;

protected:
    ImportType m_importType;           // 导入类型
    ImportPathType m_pathType;         // 路径类型
    std::string m_importPath;          // 导入路径
    std::string m_symbolName;          // 符号名称
    std::string m_symbolType;          // 符号类型
    std::string m_alias;               // 别名
    std::string m_sourceNamespace;     // 源命名空间
    std::vector<std::string> m_importedSymbols; // 导入的符号列表
    
    /**
     * 获取对应的节点类型
     */
    CHTLNodeType getImportNodeType() const;
    
    /**
     * 获取默认文件扩展名
     */
    std::string getDefaultExtension() const;
};

/**
 * HTML导入节点
 */
class HtmlImportNode : public ImportNode {
public:
    HtmlImportNode() : ImportNode(ImportType::HTML_IMPORT) {}
    CHTLNodeType getNodeType() const { return CHTLNodeType::IMPORT_HTML_NODE; }
};

/**
 * 样式导入节点
 */
class StyleImportNode : public ImportNode {
public:
    StyleImportNode() : ImportNode(ImportType::STYLE_IMPORT) {}
    CHTLNodeType getNodeType() const { return CHTLNodeType::IMPORT_STYLE_NODE; }
};

/**
 * JavaScript导入节点
 */
class JavaScriptImportNode : public ImportNode {
public:
    JavaScriptImportNode() : ImportNode(ImportType::JAVASCRIPT_IMPORT) {}
    CHTLNodeType getNodeType() const { return CHTLNodeType::IMPORT_JAVASCRIPT_NODE; }
};

/**
 * CHTL导入节点
 */
class CHTLImportNode : public ImportNode {
public:
    CHTLImportNode() : ImportNode(ImportType::CHTL_IMPORT) {}
    CHTLNodeType getNodeType() const { return CHTLNodeType::IMPORT_CHTL_NODE; }
};

/**
 * CJMOD导入节点
 */
class CJMODImportNode : public ImportNode {
public:
    CJMODImportNode() : ImportNode(ImportType::CJMOD_IMPORT) {}
    CHTLNodeType getNodeType() const { return CHTLNodeType::IMPORT_CJMOD_NODE; }
};

/**
 * 配置导入节点
 */
class ConfigImportNode : public ImportNode {
public:
    ConfigImportNode() : ImportNode(ImportType::CONFIG_IMPORT) {}
    CHTLNodeType getNodeType() const { return CHTLNodeType::IMPORT_CONFIG_NODE; }
};

/**
 * 模板导入节点
 */
class TemplateImportNode : public ImportNode {
public:
    TemplateImportNode() : ImportNode(ImportType::TEMPLATE_IMPORT) {}
    CHTLNodeType getNodeType() const { return CHTLNodeType::IMPORT_TEMPLATE_NODE; }
};

/**
 * 自定义导入节点
 */
class CustomImportNode : public ImportNode {
public:
    CustomImportNode() : ImportNode(ImportType::CUSTOM_IMPORT) {}
    CHTLNodeType getNodeType() const { return CHTLNodeType::IMPORT_CUSTOM_NODE; }
};

/**
 * 原始嵌入导入节点
 */
class OriginImportNode : public ImportNode {
public:
    OriginImportNode() : ImportNode(ImportType::ORIGIN_IMPORT) {}
    CHTLNodeType getNodeType() const { return CHTLNodeType::IMPORT_ORIGIN_NODE; }
};

/**
 * 导入管理器
 * 负责处理导入解析、路径搜索、模块加载等
 */
class ImportManager {
public:
    /**
     * 构造函数
     */
    ImportManager();
    
    /**
     * 析构函数
     */
    ~ImportManager() = default;
    
    /**
     * 设置官方模块目录
     */
    void setOfficialModuleDirectory(const std::string& directory);
    
    /**
     * 添加搜索路径
     */
    void addSearchPath(const std::string& path);
    
    /**
     * 解析导入路径
     */
    std::string resolveImportPath(const ImportNode& importNode, const std::string& currentFile) const;
    
    /**
     * 加载模块
     */
    bool loadModule(const std::string& modulePath);
    
    /**
     * 检查文件是否存在
     */
    bool fileExists(const std::string& filePath) const;
    
    /**
     * 获取文件扩展名
     */
    std::string getFileExtension(const ImportType& importType) const;
    
    /**
     * 搜索模块文件
     */
    std::string searchModuleFile(const std::string& moduleName, const ImportType& importType) const;
    
    /**
     * 处理导入语句
     */
    bool processImport(std::shared_ptr<ImportNode> importNode, const std::string& currentFile);
    
    /**
     * 获取已导入的模块列表
     */
    const std::unordered_set<std::string>& getLoadedModules() const;
    
    /**
     * 检查循环导入
     */
    bool hasCircularImport(const std::string& filePath) const;
    
    /**
     * 加载文件内容
     */
    std::string loadFileContent(const std::string& filePath) const;
    
    /**
     * 检查循环依赖
     */
    bool hasCircularDependency(const std::string& filePath) const;
    
    /**
     * 添加依赖关系
     */
    void addDependency(const std::string& from, const std::string& to);
    
    /**
     * 检查模块目录结构
     */
    bool checkModuleStructure(const std::string& directory) const;
    
    /**
     * 获取模块信息
     */
    std::unordered_map<std::string, std::string> getModuleInfo(const std::string& modulePath) const;
    
    /**
     * 解析带名原始嵌入
     */
    std::string createNamedOriginEmbed(const std::string& content, const std::string& type, const std::string& name) const;

private:
    std::string m_officialModuleDirectory;          // 官方模块目录
    std::vector<std::string> m_searchPaths;         // 搜索路径
    std::unordered_set<std::string> m_loadedModules; // 已加载的模块
    std::unordered_map<std::string, std::string> m_moduleCache; // 模块缓存
    std::unordered_map<std::string, std::vector<std::string>> m_dependencies; // 依赖关系图
    std::unordered_set<std::string> m_currentlyLoading; // 当前正在加载的文件（用于循环依赖检测）
    
    /**
     * 获取搜索路径
     */
    std::vector<std::string> getSearchPaths(const std::string& currentFile) const;
    
    /**
     * 搜索文件
     */
    std::string searchFile(const std::string& filename, const std::vector<std::string>& searchDirs) const;
    
    /**
     * 规范化路径
     */
    std::string normalizePath(const std::string& path) const;
    
    /**
     * 解析CMOD模块
     */
    bool loadCMODModule(const std::string& modulePath);
    
    /**
     * 解析CJMOD模块
     */
    bool loadCJMODModule(const std::string& modulePath);
};



} // namespace CHTL