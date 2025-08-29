#ifndef CHTLJS_CJMOD_LOADER_H
#define CHTLJS_CJMOD_LOADER_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace CHTLJS {

// CJMOD扩展信息
struct CJMODExtension {
    std::string name;
    std::string version;
    std::string entryPoint;
    std::vector<std::string> dependencies;
};

// CJMOD加载器
class CJMODLoader {
public:
    CJMODLoader();
    ~CJMODLoader();
    
    // 加载CJMOD扩展
    bool loadExtension(const std::string& path);
    
    // 卸载扩展
    bool unloadExtension(const std::string& name);
    
    // 获取已加载的扩展
    std::vector<std::string> getLoadedExtensions() const;
    
    // 检查扩展是否已加载
    bool isExtensionLoaded(const std::string& name) const;
    
    // 获取扩展信息
    std::shared_ptr<CJMODExtension> getExtensionInfo(const std::string& name) const;
    
private:
    std::unordered_map<std::string, std::shared_ptr<CJMODExtension>> extensions_;
    
    // 验证扩展
    bool validateExtension(const CJMODExtension& ext);
    
    // 解析扩展清单
    std::shared_ptr<CJMODExtension> parseManifest(const std::string& manifestPath);
};

} // namespace CHTLJS

#endif // CHTLJS_CJMOD_LOADER_H