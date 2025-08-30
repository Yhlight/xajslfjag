#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

/**
 * CMOD信息结构
 */
struct CMODInfo {
    std::string name;           // 模块名称
    std::string version;        // 版本
    std::string description;    // 描述
    std::vector<std::string> dependencies; // 依赖
    std::string author;         // 作者
    std::string license;        // 许可证
    
    CMODInfo() = default;
};

/**
 * CMOD打包器
 */
class CMODPacker {
public:
    CMODPacker();
    ~CMODPacker() = default;
    
    /**
     * 打包CMOD文件
     */
    bool packCMOD(const std::string& sourceDir, const std::string& outputPath, const CMODInfo& info);
    
    /**
     * 解包CMOD文件
     */
    bool unpackCMOD(const std::string& cmodPath, const std::string& outputDir);
    
    /**
     * 验证CMOD文件
     */
    bool validateCMOD(const std::string& cmodPath, CMODInfo& info);

private:
    /**
     * 生成info文件
     */
    std::string generateInfoFile(const CMODInfo& info);
    
    /**
     * 解析info文件
     */
    CMODInfo parseInfoFile(const std::string& infoContent);
};

/**
 * CMOD管理器
 * 管理CMOD模块的加载和依赖解析
 */
class CMODManager {
public:
    CMODManager();
    ~CMODManager() = default;
    
    /**
     * 加载CMOD模块
     */
    bool loadCMOD(const std::string& modulePath);
    
    /**
     * 获取模块信息
     */
    CMODInfo getModuleInfo(const std::string& moduleName) const;
    
    /**
     * 解析模块依赖
     */
    std::vector<std::string> resolveDependencies(const std::string& moduleName);
    
    /**
     * 检查循环依赖
     */
    bool hasCircularDependency(const std::string& moduleName);

private:
    std::unordered_map<std::string, CMODInfo> m_loadedModules;
    
    /**
     * 递归检查循环依赖
     */
    bool checkCircularDependencyRecursive(const std::string& moduleName, 
                                         std::unordered_set<std::string>& visited,
                                         std::unordered_set<std::string>& recursionStack);
};

} // namespace CHTL