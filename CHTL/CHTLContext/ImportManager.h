#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "ConfigurationManager.h"

namespace CHTL {

// 导入类型
enum class ImportType {
    HTML,           // @Html
    STYLE,          // @Style
    JAVASCRIPT,     // @JavaScript
    CHTL,           // @Chtl
    CJMOD,          // @CJmod
    CONFIG,         // @Config
    CUSTOM_ELEMENT, // [Custom] @Element
    CUSTOM_STYLE,   // [Custom] @Style
    CUSTOM_VAR,     // [Custom] @Var
    TEMPLATE_ELEMENT, // [Template] @Element
    TEMPLATE_STYLE,   // [Template] @Style
    TEMPLATE_VAR,     // [Template] @Var
    ORIGIN_HTML,    // [Origin] @Html
    ORIGIN_STYLE,   // [Origin] @Style
    ORIGIN_JAVASCRIPT, // [Origin] @JavaScript
    ORIGIN_CUSTOM   // [Origin] @Custom
};

// 导入信息
struct ImportInfo {
    ImportType type;
    std::string path;
    std::string alias;
    std::string sourceFile;
    size_t line;
    bool isWildcard;
    std::vector<std::string> subModules;
    std::string targetName;  // 对于模板/自定义/原始嵌入类型
    
    ImportInfo(ImportType type, const std::string& path, const std::string& alias = "",
               const std::string& sourceFile = "", size_t line = 0)
        : type(type), path(path), alias(alias), sourceFile(sourceFile), line(line), isWildcard(false) {}
};

// 路径解析结果
struct PathResolutionResult {
    std::vector<std::string> resolvedPaths;
    std::string error;
    bool success;
    
    PathResolutionResult() : success(false) {}
};

// 导入管理器
class ImportManager {
public:
    ImportManager(std::shared_ptr<ConfigurationManager> configManager, const std::string& basePath = "");
    ~ImportManager() = default;
    
    // 导入管理
    void addImport(const ImportInfo& import);
    std::vector<ImportInfo> getImports() const;
    bool removeImport(const ImportInfo& import);
    
    // 路径解析
    PathResolutionResult resolvePath(const std::string& path, ImportType type);
    std::vector<std::string> resolveWildcardPath(const std::string& path, ImportType type);
    std::vector<std::string> resolveSubModulePath(const std::string& path, ImportType type);
    
    // 循环依赖检测
    bool checkCircularDependency(const std::string& filePath);
    std::vector<std::string> getCircularDependencies(const std::string& filePath);
    
    // 重复导入检测
    bool checkDuplicateImport(const std::string& type, const std::string& path);
    std::vector<ImportInfo> getDuplicateImports() const;
    
    // 搜索路径管理
    void addSearchPath(const std::string& path);
    void removeSearchPath(const std::string& path);
    std::vector<std::string> getSearchPaths() const;
    
    // 模块搜索
    std::vector<std::string> searchOfficialModules(const std::string& name, ImportType type);
    std::vector<std::string> searchLocalModules(const std::string& name, ImportType type);
    std::vector<std::string> searchCurrentDirectory(const std::string& name, ImportType type);
    
    // 文件类型检测
    bool isValidFileForType(const std::string& filePath, ImportType type);
    std::vector<std::string> getValidExtensionsForType(ImportType type);
    
    // 通配符处理
    std::vector<std::string> expandWildcard(const std::string& pattern, ImportType type);
    bool isWildcardPattern(const std::string& pattern);
    
    // 子模块处理
    std::vector<std::string> parseSubModulePath(const std::string& path);
    std::string normalizeSubModulePath(const std::string& path);
    
    // 别名管理
    void addAlias(const std::string& alias, const std::string& path);
    std::string resolveAlias(const std::string& alias);
    bool hasAlias(const std::string& alias) const;
    
    // 导入验证
    bool validateImport(const ImportInfo& import);
    std::vector<std::string> getImportErrors(const ImportInfo& import);
    
    // 导入统计
    struct ImportStatistics {
        size_t totalImports;
        size_t successfulImports;
        size_t failedImports;
        size_t circularDependencies;
        size_t duplicateImports;
        
        ImportStatistics() : totalImports(0), successfulImports(0), failedImports(0),
                            circularDependencies(0), duplicateImports(0) {}
    };
    
    const ImportStatistics& getStatistics() const { return statistics_; }
    void clearStatistics();
    
    // 调试信息
    std::string getDebugInfo() const;
    std::string getImportInfo() const;
    
    // 重置
    void reset();
    
private:
    std::shared_ptr<ConfigurationManager> configManager_;
    std::string basePath_;
    std::vector<ImportInfo> imports_;
    std::vector<std::string> searchPaths_;
    std::unordered_map<std::string, std::string> aliases_;
    std::unordered_set<std::string> processedFiles_;
    ImportStatistics statistics_;
    
    // 辅助函数
    std::string normalizePath(const std::string& path);
    std::string getDirectoryFromPath(const std::string& path);
    std::string getFileNameFromPath(const std::string& path);
    std::string getExtensionFromPath(const std::string& path);
    
    // 路径搜索
    std::vector<std::string> searchInDirectory(const std::string& directory, const std::string& pattern, ImportType type);
    std::vector<std::string> searchInModuleDirectory(const std::string& modulePath, const std::string& name, ImportType type);
    
    // 依赖检测
    void buildDependencyGraph();
    bool hasCircularDependencyDFS(const std::string& file, std::unordered_set<std::string>& visited, 
                                 std::unordered_set<std::string>& recursionStack);
    
    // 文件操作
    bool fileExists(const std::string& path);
    bool isDirectory(const std::string& path);
    std::vector<std::string> listFiles(const std::string& directory, const std::string& pattern);
    
    // 统计更新
    void updateStatistics(const std::string& type, size_t value = 1);
    
    // 配置相关
    std::string getOfficialModulePath() const;
    std::string getLocalModulePath() const;
    bool isOfficialModulePrefix(const std::string& path) const;
};

} // namespace CHTL