#pragma once
#include "../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <fstream>

namespace CHTL {

/**
 * ZIP文件条目信息
 */
struct ZipEntry {
    std::string filename;               // 文件名
    std::string fullPath;               // 完整路径
    size_t compressedSize;              // 压缩后大小
    size_t uncompressedSize;            // 未压缩大小
    uint32_t crc32;                     // CRC32校验码
    std::vector<uint8_t> data;          // 文件数据
    bool isDirectory;                   // 是否为目录
    
    ZipEntry() : compressedSize(0), uncompressedSize(0), crc32(0), isDirectory(false) {}
};

/**
 * CMOD模块信息
 */
struct CMODInfo {
    std::string name;                   // 模块名
    std::string version;                // 版本
    std::string description;            // 描述
    std::vector<std::string> dependencies; // 依赖
    std::unordered_map<std::string, std::string> metadata; // 元数据
    std::vector<std::string> exports;   // 导出列表
    std::string mainFile;               // 主文件
    
    CMODInfo() = default;
};

/**
 * CJMOD扩展信息
 */
struct CJMODInfo : public CMODInfo {
    std::vector<std::string> jsFiles;   // JavaScript文件列表
    std::vector<std::string> apiFiles;  // API文件列表
    std::unordered_map<std::string, std::string> bindings; // 绑定信息
    
    CJMODInfo() = default;
};

/**
 * 简化的ZIP文件处理器
 * 提供基本的ZIP压缩/解压功能，无需依赖外部库
 */
class SimpleZipHandler {
public:
    /**
     * 创建ZIP文件
     */
    bool createZip(const std::string& zipPath, const std::vector<ZipEntry>& entries);
    
    /**
     * 解压ZIP文件
     */
    bool extractZip(const std::string& zipPath, std::vector<ZipEntry>& entries);
    
    /**
     * 添加文件到ZIP
     */
    bool addFileToZip(const std::string& filePath, const std::string& archivePath, ZipEntry& entry);
    
    /**
     * 从ZIP中提取文件
     */
    bool extractFileFromZip(const ZipEntry& entry, const std::string& outputPath);
    
    /**
     * 验证ZIP文件
     */
    bool validateZip(const std::string& zipPath);
    
    /**
     * 获取ZIP文件信息
     */
    bool getZipInfo(const std::string& zipPath, std::vector<std::string>& fileList);

public:
    /**
     * 计算CRC32
     */
    uint32_t calculateCRC32(const std::vector<uint8_t>& data);
    
    /**
     * 简单压缩（DEFLATE算法的简化版本）
     */
    std::vector<uint8_t> simpleCompress(const std::vector<uint8_t>& data);
    
    /**
     * 简单解压
     */
    std::vector<uint8_t> simpleDecompress(const std::vector<uint8_t>& compressedData, size_t originalSize);
    
    /**
     * 写入ZIP文件头
     */
    void writeZipHeader(std::ofstream& file, const ZipEntry& entry);
    
    /**
     * 读取ZIP文件头
     */
    bool readZipHeader(std::ifstream& file, ZipEntry& entry);
    
    /**
     * 写入中央目录
     */
    void writeCentralDirectory(std::ofstream& file, const std::vector<ZipEntry>& entries);
    
    /**
     * 读取中央目录
     */
    bool readCentralDirectory(std::ifstream& file, std::vector<ZipEntry>& entries);
};

/**
 * CMOD模块打包器
 */
class CMODPacker {
public:
    /**
     * 打包CMOD模块
     */
    bool packCMOD(const std::string& sourcePath, const std::string& cmodPath, const CMODInfo& info);
    
    /**
     * 解包CMOD模块
     */
    bool unpackCMOD(const std::string& cmodPath, const std::string& targetPath, CMODInfo& info);
    
    /**
     * 验证CMOD文件
     */
    bool validateCMOD(const std::string& cmodPath, CMODInfo& info);
    
    /**
     * 生成模块信息文件
     */
    std::string generateModuleInfo(const CMODInfo& info);
    
    /**
     * 解析模块信息文件
     */
    bool parseModuleInfo(const std::string& content, CMODInfo& info);

private:
    std::unique_ptr<SimpleZipHandler> m_zipHandler;
    
    /**
     * 收集源文件
     */
    std::vector<std::string> collectSourceFiles(const std::string& sourcePath);
    
    /**
     * 过滤文件类型
     */
    bool shouldIncludeFile(const std::string& filename);
    
    /**
     * 生成依赖图
     */
    bool generateDependencyGraph(const std::vector<std::string>& files, std::vector<std::string>& dependencies);
};

/**
 * CJMOD模块打包器
 */
class CJMODPacker {
public:
    CJMODPacker();
    /**
     * 打包CJMOD模块
     */
    bool packCJMOD(const std::string& sourcePath, const std::string& cjmodPath, const CJMODInfo& info);
    
    /**
     * 解包CJMOD模块
     */
    bool unpackCJMOD(const std::string& cjmodPath, const std::string& targetPath, CJMODInfo& info);
    
    /**
     * 验证CJMOD文件
     */
    bool validateCJMOD(const std::string& cjmodPath, CJMODInfo& info);
    
    /**
     * 生成扩展信息文件
     */
    std::string generateExtensionInfo(const CJMODInfo& info);
    
    /**
     * 解析扩展信息文件
     */
    bool parseExtensionInfo(const std::string& content, CJMODInfo& info);
    
    /**
     * 处理JavaScript绑定
     */
    bool processJavaScriptBindings(const std::vector<std::string>& jsFiles, CJMODInfo& info);

private:
    std::unique_ptr<SimpleZipHandler> m_zipHandler;
    std::unique_ptr<CMODPacker> m_basePacker;
    
    /**
     * 收集JavaScript文件
     */
    std::vector<std::string> collectJavaScriptFiles(const std::string& sourcePath);
    
    /**
     * 分析API绑定
     */
    bool analyzeAPIBindings(const std::string& jsFile, std::unordered_map<std::string, std::string>& bindings);
    
    /**
     * 生成绑定代码
     */
    std::string generateBindingCode(const CJMODInfo& info);
};

/**
 * 模块依赖解析器
 */
class ModuleDependencyResolver {
public:
    /**
     * 解析依赖图
     */
    bool resolveDependencies(const std::vector<std::string>& modules, 
                           std::vector<std::string>& resolvedOrder);
    
    /**
     * 检查循环依赖
     */
    bool checkCircularDependencies(const std::vector<std::string>& modules, 
                                 std::vector<std::string>& circularPaths);
    
    /**
     * 获取模块依赖
     */
    std::vector<std::string> getModuleDependencies(const std::string& modulePath);
    
    /**
     * 验证依赖完整性
     */
    bool validateDependencies(const std::vector<std::string>& modules, 
                            std::vector<std::string>& missingDependencies);

private:
    /**
     * 构建依赖图
     */
    void buildDependencyGraph(const std::vector<std::string>& modules,
                            std::unordered_map<std::string, std::vector<std::string>>& graph);
    
    /**
     * 拓扑排序
     */
    bool topologicalSort(const std::unordered_map<std::string, std::vector<std::string>>& graph,
                       std::vector<std::string>& result);
    
    /**
     * DFS检查环
     */
    bool hasCycleDFS(const std::string& node,
                   const std::unordered_map<std::string, std::vector<std::string>>& graph,
                   std::unordered_map<std::string, int>& state,
                   std::vector<std::string>& path);
};

/**
 * 增强的模块管理器
 * 支持ZIP格式的CMOD/CJMOD模块
 */
class ZipModuleManager {
public:
    ZipModuleManager();
    ~ZipModuleManager() = default;
    
    /**
     * 打包CMOD模块
     */
    bool packCMOD(const std::string& sourcePath, const std::string& outputPath, const CMODInfo& info);
    
    /**
     * 解包CMOD模块
     */
    bool unpackCMOD(const std::string& cmodPath, const std::string& targetPath);
    
    /**
     * 打包CJMOD模块
     */
    bool packCJMOD(const std::string& sourcePath, const std::string& outputPath, const CJMODInfo& info);
    
    /**
     * 解包CJMOD模块
     */
    bool unpackCJMOD(const std::string& cjmodPath, const std::string& targetPath);
    
    /**
     * 安装模块
     */
    bool installModule(const std::string& modulePath, const std::string& installPath);
    
    /**
     * 卸载模块
     */
    bool uninstallModule(const std::string& moduleName, const std::string& installPath);
    
    /**
     * 验证模块
     */
    bool validateModule(const std::string& modulePath);
    
    /**
     * 解析依赖
     */
    bool resolveDependencies(const std::vector<std::string>& modules, std::vector<std::string>& order);
    
    /**
     * 获取模块信息
     */
    bool getModuleInfo(const std::string& modulePath, CMODInfo& info);
    
    /**
     * 搜索模块
     */
    std::vector<std::string> searchModules(const std::string& searchPath, const std::string& pattern);
    
    /**
     * 获取已安装模块列表
     */
    std::vector<std::string> getInstalledModules(const std::string& installPath);
    
    /**
     * 设置模块搜索路径
     */
    void addModuleSearchPath(const std::string& path);
    
    /**
     * 获取管理统计
     */
    std::string getManagerStats() const;
    
    /**
     * 清理状态
     */
    void cleanup();

private:
    std::unique_ptr<CMODPacker> m_cmodPacker;
    std::unique_ptr<CJMODPacker> m_cjmodPacker;
    std::unique_ptr<ModuleDependencyResolver> m_dependencyResolver;
    
    std::vector<std::string> m_searchPaths;
    std::unordered_map<std::string, CMODInfo> m_moduleCache;
    std::unordered_map<std::string, int> m_stats;
    
    /**
     * 更新统计信息
     */
    void updateStats(const std::string& category);
    
    /**
     * 缓存模块信息
     */
    void cacheModuleInfo(const std::string& modulePath, const CMODInfo& info);
    
    /**
     * 查找模块文件
     */
    std::string findModuleFile(const std::string& moduleName);
    
    /**
     * 验证模块完整性
     */
    bool verifyModuleIntegrity(const std::string& modulePath);
    
    /**
     * 报告错误
     */
    void reportError(const std::string& message);
};

} // namespace CHTL