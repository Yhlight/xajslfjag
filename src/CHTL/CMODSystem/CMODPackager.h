#ifndef CHTL_CMOD_PACKAGER_H
#define CHTL_CMOD_PACKAGER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>

namespace CHTL {

// CMOD文件结构
struct CMODStructure {
    std::string moduleName;
    std::string version;
    
    // 主模块文件
    std::optional<std::string> mainModuleFile;
    
    // 子模块
    struct SubModule {
        std::string name;
        std::string srcPath;
        std::string infoPath;
    };
    std::vector<SubModule> subModules;
    
    // 源文件
    std::vector<std::string> sourceFiles;
    
    // info目录内容
    std::string infoContent;
    
    // 元数据
    std::unordered_map<std::string, std::string> metadata;
};

// CMOD信息
struct CMODInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string dependencies;
    std::string category;
    std::string minCHTLVersion;
    std::string maxCHTLVersion;
};

// CMOD导出信息
struct CMODExport {
    std::vector<std::string> customStyles;
    std::vector<std::string> customElements;
    std::vector<std::string> customVars;
    std::vector<std::string> templateStyles;
    std::vector<std::string> templateElements;
    std::vector<std::string> templateVars;
    std::vector<std::string> originHtml;
    std::vector<std::string> originStyle;
    std::vector<std::string> originJavascript;
    std::unordered_map<std::string, std::vector<std::string>> customOriginTypes;
    std::vector<std::string> configurations;
};

// CMOD打包器
class CMODPackager {
public:
    CMODPackager();
    ~CMODPackager() = default;
    
    // 设置压缩级别（0-9）
    void setCompressionLevel(int level) { compressionLevel_ = level; }
    
    // 打包CMOD
    bool package(const std::string& sourceDir, const std::string& outputFile);
    
    // 解包CMOD
    bool unpack(const std::string& cmodFile, const std::string& outputDir);
    
    // 验证CMOD结构
    bool validateStructure(const std::string& sourceDir);
    
    // 获取CMOD信息（不解包）
    std::optional<CMODInfo> getInfo(const std::string& cmodFile);
    
    // 获取导出信息（不解包）
    std::optional<CMODExport> getExports(const std::string& cmodFile);
    
    // 获取错误信息
    const std::string& getLastError() const { return lastError_; }

private:
    int compressionLevel_ = 6;
    std::string lastError_;
    
    // 内部方法
    bool analyzeDirectory(const std::string& dir, CMODStructure& structure);
    bool validateModuleStructure(const CMODStructure& structure);
    bool parseInfoFile(const std::string& infoPath, CMODInfo& info);
    bool parseExportInfo(const std::string& content, CMODExport& exports);
    
    // 文件操作
    bool createZipArchive(const CMODStructure& structure, const std::string& outputFile);
    bool extractZipArchive(const std::string& zipFile, const std::string& outputDir);
    
    // 辅助方法
    std::string readFile(const std::string& path);
    bool writeFile(const std::string& path, const std::string& content);
    bool copyFile(const std::string& src, const std::string& dst);
    bool createDirectory(const std::string& path);
    bool isValidModuleName(const std::string& name);
    std::string generateManifest(const CMODStructure& structure);
    bool parseManifest(const std::string& manifest, CMODStructure& structure);
};

// CMOD加载器
class CMODLoader {
public:
    static CMODLoader& getInstance() {
        static CMODLoader instance;
        return instance;
    }
    
    // 加载CMOD模块
    bool loadModule(const std::string& cmodFile);
    
    // 从目录加载模块
    bool loadFromDirectory(const std::string& dir);
    
    // 获取已加载的模块
    std::shared_ptr<CMODStructure> getModule(const std::string& moduleName) const;
    
    // 检查模块是否已加载
    bool isModuleLoaded(const std::string& moduleName) const;
    
    // 获取模块路径
    std::optional<std::string> getModulePath(const std::string& moduleName) const;
    
    // 卸载模块
    void unloadModule(const std::string& moduleName);
    
    // 清空所有模块
    void clearAll();

private:
    CMODLoader() = default;
    CMODLoader(const CMODLoader&) = delete;
    CMODLoader& operator=(const CMODLoader&) = delete;
    
    // 已加载的模块
    std::unordered_map<std::string, std::shared_ptr<CMODStructure>> loadedModules_;
    
    // 模块路径映射
    std::unordered_map<std::string, std::string> modulePaths_;
};

} // namespace CHTL

#endif // CHTL_CMOD_PACKAGER_H