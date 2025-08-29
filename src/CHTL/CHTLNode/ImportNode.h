#pragma once

#include "BaseNode.h"

namespace CHTL {

// 导入节点基类
class ImportNode : public BaseNode {
public:
    String importType;      // Html, Style, JavaScript, Chtl, CJmod等
    String sourcePath;      // 源路径
    String aliasName;       // 别名（as语法）
    bool hasAlias;          // 是否有别名
    StringVector importItems;  // 具体导入项
    StringVector exceptItems;  // 排除项
    bool isWildcardImport;  // 是否是通配符导入
    bool isResolved;        // 是否已解析
    String resolvedPath;    // 解析后的路径
    StringUnorderedMap importMetadata;  // 导入元数据
    
    ImportNode(NodeType type, const String& iType, const String& source, const Position& pos = Position())
        : BaseNode(type, source, pos), importType(iType), sourcePath(source), 
          hasAlias(false), isWildcardImport(false), isResolved(false) {}
    
    // 基础操作
    void setSourcePath(const String& path) { sourcePath = path; value = path; }
    String getSourcePath() const { return sourcePath; }
    
    void setAlias(const String& alias) { 
        aliasName = alias; 
        hasAlias = !alias.empty(); 
    }
    String getAlias() const { return aliasName; }
    bool hasAliasName() const { return hasAlias; }
    
    void setImportType(const String& type) { importType = type; }
    String getImportType() const { return importType; }
    
    // 导入项管理
    void addImportItem(const String& item);
    void removeImportItem(const String& item);
    bool hasImportItem(const String& item) const;
    StringVector getImportItems() const { return importItems; }
    void clearImportItems() { importItems.clear(); }
    
    // 排除项管理
    void addExceptItem(const String& item);
    void removeExceptItem(const String& item);
    bool hasExceptItem(const String& item) const;
    StringVector getExceptItems() const { return exceptItems; }
    void clearExceptItems() { exceptItems.clear(); }
    
    // 通配符导入
    void setWildcardImport(bool wildcard) { isWildcardImport = wildcard; }
    bool isWildcard() const { return isWildcardImport; }
    
    // 路径解析
    void setResolvedPath(const String& path) { resolvedPath = path; isResolved = true; }
    String getResolvedPath() const { return resolvedPath; }
    bool pathIsResolved() const { return isResolved; }
    
    // 元数据管理
    void setImportMetadata(const String& key, const String& value) { importMetadata[key] = value; }
    String getImportMetadata(const String& key, const String& defaultValue = "") const {
        auto it = importMetadata.find(key);
        return (it != importMetadata.end()) ? it->second : defaultValue;
    }
    bool hasImportMetadata(const String& key) const { return importMetadata.find(key) != importMetadata.end(); }
    
    // 类型检查
    bool isFileImport() const;
    bool isModuleImport() const;
    bool isTemplateImport() const;
    bool isCustomImport() const;
    bool isOriginImport() const;
    bool isConfigImport() const;
    
    // 验证
    virtual bool validateImport() const;
    virtual StringVector getImportValidationErrors() const;
    
    // 生成导入语句
    virtual String generateImportStatement() const;
    
    // 重写基类方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
    bool validate() const override;
    StringVector getValidationErrors() const override;
};

// HTML文件导入
class HtmlImportNode : public ImportNode {
public:
    explicit HtmlImportNode(const String& source, const String& alias = "", const Position& pos = Position())
        : ImportNode(NodeType::IMPORT_HTML, "Html", source, pos) {
        if (!alias.empty()) setAlias(alias);
    }
    
    // HTML特定验证
    bool validateImport() const override;
    StringVector getImportValidationErrors() const override;
    
    // HTML处理
    bool isValidHtmlFile() const;
    String extractTitle() const;
    StringVector extractMetaTags() const;
    StringVector extractLinks() const;
    StringVector extractScripts() const;
    
    // 生成
    String generateImportStatement() const override;
    String generateIncludeStatement() const;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// CSS文件导入
class StyleImportNode : public ImportNode {
public:
    explicit StyleImportNode(const String& source, const String& alias = "", const Position& pos = Position())
        : ImportNode(NodeType::IMPORT_STYLE, "Style", source, pos) {
        if (!alias.empty()) setAlias(alias);
    }
    
    // CSS特定验证
    bool validateImport() const override;
    StringVector getImportValidationErrors() const override;
    
    // CSS处理
    bool isValidCssFile() const;
    StringVector extractSelectors() const;
    StringVector extractMediaQueries() const;
    StringVector extractImports() const;  // CSS @import
    
    // 生成
    String generateImportStatement() const override;
    String generateLinkStatement() const;
    String generateInlineStyle() const;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// JavaScript文件导入
class JavaScriptImportNode : public ImportNode {
public:
    explicit JavaScriptImportNode(const String& source, const String& alias = "", const Position& pos = Position())
        : ImportNode(NodeType::IMPORT_JAVASCRIPT, "JavaScript", source, pos) {
        if (!alias.empty()) setAlias(alias);
    }
    
    // JavaScript特定验证
    bool validateImport() const override;
    StringVector getImportValidationErrors() const override;
    
    // JavaScript处理
    bool isValidJsFile() const;
    StringVector extractExports() const;
    StringVector extractImports() const;
    bool isESModule() const;
    bool isCommonJS() const;
    
    // 生成
    String generateImportStatement() const override;
    String generateScriptTag() const;
    String generateESImport() const;
    String generateCommonJSRequire() const;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// CHTL文件导入
class CHTLImportNode : public ImportNode {
public:
    explicit CHTLImportNode(const String& source, const String& alias = "", const Position& pos = Position())
        : ImportNode(NodeType::IMPORT_CHTL, "Chtl", source, pos) {
        if (!alias.empty()) setAlias(alias);
    }
    
    // CHTL特定验证
    bool validateImport() const override;
    StringVector getImportValidationErrors() const override;
    
    // CHTL处理
    bool isValidChtlFile() const;
    bool isCmodFile() const;
    StringVector extractTemplates() const;
    StringVector extractCustoms() const;
    StringVector extractOrigins() const;
    StringVector extractNamespaces() const;
    
    // 子模块支持
    bool hasSubmodules() const;
    StringVector getSubmodules() const;
    String getSubmodulePath(const String& submodule) const;
    
    // 生成
    String generateImportStatement() const override;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// CJMOD导入
class CJMODImportNode : public ImportNode {
public:
    explicit CJMODImportNode(const String& source, const String& alias = "", const Position& pos = Position())
        : ImportNode(NodeType::IMPORT_CJMOD, "CJmod", source, pos) {
        if (!alias.empty()) setAlias(alias);
    }
    
    // CJMOD特定验证
    bool validateImport() const override;
    StringVector getImportValidationErrors() const override;
    
    // CJMOD处理
    bool isValidCjmodFile() const;
    StringVector extractFunctions() const;
    StringVector extractSyntaxExtensions() const;
    
    // 生成
    String generateImportStatement() const override;
    String generateLoadStatement() const;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// 配置导入
class ConfigImportNode : public ImportNode {
public:
    String configName;  // 配置名称
    
    explicit ConfigImportNode(const String& source, const String& configName = "", const String& alias = "", const Position& pos = Position())
        : ImportNode(NodeType::IMPORT_CONFIG, "Config", source, pos), configName(configName) {
        if (!alias.empty()) setAlias(alias);
    }
    
    void setConfigName(const String& name) { configName = name; }
    String getConfigName() const { return configName; }
    
    // 配置特定验证
    bool validateImport() const override;
    StringVector getImportValidationErrors() const override;
    
    // 配置处理
    bool isValidConfigFile() const;
    StringUnorderedMap extractConfigValues() const;
    
    // 生成
    String generateImportStatement() const override;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// 原始嵌入导入
class OriginImportNode : public ImportNode {
public:
    String originType;   // Html, Style, JavaScript等
    String originName;   // 原始嵌入名称
    
    explicit OriginImportNode(const String& type, const String& name, const String& source, const String& alias = "", const Position& pos = Position())
        : ImportNode(NodeType::IMPORT_ORIGIN, "Origin", source, pos), originType(type), originName(name) {
        if (!alias.empty()) setAlias(alias);
    }
    
    void setOriginType(const String& type) { originType = type; }
    String getOriginType() const { return originType; }
    
    void setOriginName(const String& name) { originName = name; }
    String getOriginName() const { return originName; }
    
    // 原始嵌入特定验证
    bool validateImport() const override;
    StringVector getImportValidationErrors() const override;
    
    // 生成
    String generateImportStatement() const override;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
};

// 导入解析器
class ImportResolver {
public:
    struct ResolveOptions {
        StringVector searchPaths;
        StringVector moduleDirectories;
        StringVector officialModulePaths;
        StringUnorderedMap pathAliases;
        bool allowWildcards = true;
        bool followSymlinks = true;
        bool cacheResults = true;
    };
    
    explicit ImportResolver(const ResolveOptions& options = ResolveOptions{});
    
    // 路径解析
    String resolvePath(const String& sourcePath, const String& currentDir = "") const;
    String resolveModulePath(const String& moduleName, const String& currentDir = "") const;
    String resolveOfficialModule(const String& moduleName) const;
    
    // 类型解析
    String resolveImportType(const String& path) const;
    bool isValidImportPath(const String& path) const;
    
    // 通配符解析
    StringVector resolveWildcard(const String& pattern, const String& currentDir = "") const;
    StringVector resolveDirectoryImports(const String& dirPath, const String& type = "") const;
    
    // 循环依赖检测
    bool hasCircularDependency(const String& sourcePath, const String& targetPath) const;
    StringVector getDependencyChain(const String& sourcePath) const;
    
    // 缓存管理
    void clearCache();
    void addToCache(const String& path, const String& resolvedPath);
    String getFromCache(const String& path) const;
    
private:
    ResolveOptions options;
    mutable StringUnorderedMap pathCache;
    mutable std::set<String> resolvingPaths;  // 防止循环依赖
    
    String searchInPaths(const String& relativePath, const StringVector& searchPaths) const;
    String findFileWithExtensions(const String& basePath, const StringVector& extensions) const;
    bool isAbsolutePath(const String& path) const;
    bool fileExists(const String& path) const;
    bool directoryExists(const String& path) const;
    StringVector getFilesInDirectory(const String& path, const String& extension = "") const;
};

// 导入管理器
class ImportManager {
public:
    explicit ImportManager(const ImportResolver& resolver);
    
    // 导入处理
    std::unique_ptr<ImportNode> processImport(const String& importStatement, const String& currentDir = "") const;
    std::vector<std::unique_ptr<ImportNode>> processAllImports(const StringVector& importStatements, const String& currentDir = "") const;
    
    // 依赖分析
    StringVector analyzeDependencies(const String& filePath) const;
    StringUnorderedMap buildDependencyGraph(const StringVector& filePaths) const;
    StringVector getTopologicalOrder(const StringUnorderedMap& dependencyGraph) const;
    
    // 导入验证
    bool validateImports(const std::vector<std::unique_ptr<ImportNode>>& imports) const;
    StringVector getImportErrors(const std::vector<std::unique_ptr<ImportNode>>& imports) const;
    
    // 导入优化
    std::vector<std::unique_ptr<ImportNode>> optimizeImports(const std::vector<std::unique_ptr<ImportNode>>& imports) const;
    std::vector<std::unique_ptr<ImportNode>> deduplicateImports(const std::vector<std::unique_ptr<ImportNode>>& imports) const;
    
private:
    ImportResolver resolver;
    
    std::unique_ptr<ImportNode> parseImportStatement(const String& statement) const;
    String extractImportType(const String& statement) const;
    String extractSourcePath(const String& statement) const;
    String extractAlias(const String& statement) const;
    StringVector extractImportItems(const String& statement) const;
    StringVector extractExceptItems(const String& statement) const;
};

// 导入工厂
class ImportFactory {
public:
    static std::unique_ptr<ImportNode> createImport(const String& type, const String& source, const String& alias = "");
    static std::unique_ptr<HtmlImportNode> createHtmlImport(const String& source, const String& alias = "");
    static std::unique_ptr<StyleImportNode> createStyleImport(const String& source, const String& alias = "");
    static std::unique_ptr<JavaScriptImportNode> createJavaScriptImport(const String& source, const String& alias = "");
    static std::unique_ptr<CHTLImportNode> createCHTLImport(const String& source, const String& alias = "");
    static std::unique_ptr<CJMODImportNode> createCJMODImport(const String& source, const String& alias = "");
    static std::unique_ptr<ConfigImportNode> createConfigImport(const String& source, const String& configName = "", const String& alias = "");
    static std::unique_ptr<OriginImportNode> createOriginImport(const String& type, const String& name, const String& source, const String& alias = "");
    
    // 从语句创建
    static std::unique_ptr<ImportNode> createFromStatement(const String& importStatement);
    
    // 批量创建
    static std::vector<std::unique_ptr<ImportNode>> createFromStatements(const StringVector& importStatements);
};

} // namespace CHTL