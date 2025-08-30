#ifndef CHTLJS_CJMOD_PACKAGER_H
#define CHTLJS_CJMOD_PACKAGER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>
#include <map>

namespace CHTL {

// CJMOD文件结构
struct CJMODStructure {
    std::string moduleName;
    std::string version;
    
    // 主模块文件
    std::optional<std::string> mainModuleFile;
    
    // 子模块
    struct SubModule {
        std::string name;
        std::vector<std::string> srcFiles;  // C++源文件和头文件
        std::string infoPath;
    };
    std::vector<SubModule> subModules;
    
    // 源文件（C++文件）
    std::vector<std::string> sourceFiles;
    
    // info目录内容
    std::string infoContent;
    
    // 元数据
    std::unordered_map<std::string, std::string> metadata;
};

// CJMOD信息
struct CJMODInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string dependencies;
    std::string apiVersion;
    std::string minCHTLJSVersion;
    std::string maxCHTLJSVersion;
    
    // Additional fields for runtime
    bool hasExtension = false;
    std::string extensionPath;
    std::map<std::string, std::string> syntaxDefinitions;
};

// CJMOD导出信息
struct CJMODExport {
    std::vector<std::string> syntaxExtensions;     // 语法扩展列表
    std::vector<std::string> functions;             // 导出的函数
    std::vector<std::string> virtualObjects;        // 虚拟对象
    std::vector<std::string> selectors;             // 选择器扩展
    std::vector<std::string> operators;             // 操作符扩展
    std::unordered_map<std::string, std::string> bindings;  // 语法绑定
};

// CJMOD打包器
class CJMODPackager {
    friend class CJMODLoader;
public:
    CJMODPackager();
    ~CJMODPackager() = default;
    
    // 设置压缩级别（0-9）
    void setCompressionLevel(int level) { compressionLevel_ = level; }
    
    // 打包CJMOD
    bool package(const std::string& sourceDir, const std::string& outputFile);
    
    // 解包CJMOD
    bool unpack(const std::string& cjmodFile, const std::string& outputDir);
    
    // 验证CJMOD结构
    bool validateStructure(const std::string& sourceDir);
    
    // 获取CJMOD信息（不解包）
    std::optional<CJMODInfo> getInfo(const std::string& cjmodFile);
    
    // 获取导出信息（不解包）
    std::optional<CJMODExport> getExports(const std::string& cjmodFile);
    
    // 编译CJMOD（生成动态库）
    bool compile(const std::string& cjmodFile, const std::string& outputPath);
    
    // 获取错误信息
    const std::string& getLastError() const { return lastError_; }

private:
    int compressionLevel_ = 6;
    std::string lastError_;
    
    // 内部方法
    bool analyzeDirectory(const std::string& dir, CJMODStructure& structure);
    bool validateModuleStructure(const CJMODStructure& structure);
    bool parseInfoFile(const std::string& infoPath, CJMODInfo& info);
    bool parseExportInfo(const std::string& content, CJMODExport& exports);
    
    // 文件操作
    bool createZipArchive(const CJMODStructure& structure, const std::string& outputFile);
    bool extractZipArchive(const std::string& zipFile, const std::string& outputDir);
    
    // 编译相关
    bool compileCppFiles(const std::vector<std::string>& cppFiles, const std::string& outputPath);
    std::string generateBindingCode(const CJMODStructure& structure);
    
    // 解析manifest
    bool parseManifest(const std::string& manifest, CJMODStructure& structure);
    
private:
    // 辅助方法
    std::string readFile(const std::string& path);
    bool writeFile(const std::string& path, const std::string& content);
    bool isValidModuleName(const std::string& name);
    std::string generateManifest(const CJMODStructure& structure);
};

// CJMOD加载器
class CJMODLoader {
public:
    static CJMODLoader& getInstance() {
        static CJMODLoader instance;
        return instance;
    }
    
    // 加载CJMOD模块（动态库）
    bool loadModule(const std::string& cjmodPath);
    
    // 从目录加载开发中的模块
    bool loadFromDirectory(const std::string& dir);
    
    // 获取已加载的模块
    std::shared_ptr<CJMODStructure> getModule(const std::string& moduleName) const;
    
    // 检查模块是否已加载
    bool isModuleLoaded(const std::string& moduleName) const;
    
    // 获取模块路径
    std::optional<std::string> getModulePath(const std::string& moduleName) const;
    
    // 卸载模块
    void unloadModule(const std::string& moduleName);
    
    // 清空所有模块
    void clearAll();
    
    // 获取模块的API接口
    void* getModuleAPI(const std::string& moduleName, const std::string& apiName);

private:
    CJMODLoader() = default;
    CJMODLoader(const CJMODLoader&) = delete;
    CJMODLoader& operator=(const CJMODLoader&) = delete;
    
    // 已加载的模块
    std::unordered_map<std::string, std::shared_ptr<CJMODStructure>> loadedModules_;
    
    // 模块路径映射
    std::unordered_map<std::string, std::string> modulePaths_;
    
    // 动态库句柄
    std::unordered_map<std::string, void*> moduleHandles_;
};

} // namespace CHTL

#endif // CHTLJS_CJMOD_PACKAGER_H