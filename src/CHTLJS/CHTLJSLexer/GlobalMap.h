#ifndef CHTLJS_GLOBAL_MAP_H
#define CHTLJS_GLOBAL_MAP_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>
#include <optional>

namespace CHTLJS {

// 虚对象信息
class VirtualObjectInfo {
public:
    struct KeyInfo {
        std::string name;
        std::string type;      // "function", "object", "array", etc.
        std::string jsCode;    // 对应的JS代码
        
        KeyInfo() = default;  // 默认构造函数
        KeyInfo(const std::string& n, const std::string& t, const std::string& code = "")
            : name(n), type(t), jsCode(code) {}
    };
    
    VirtualObjectInfo(const std::string& name, const std::string& sourceFile)
        : name_(name), sourceFile_(sourceFile) {}
    
    const std::string& getName() const { return name_; }
    const std::string& getSourceFile() const { return sourceFile_; }
    
    // 添加键信息
    void addKey(const std::string& key, const std::string& type, const std::string& jsCode = "") {
        keys_[key] = KeyInfo(key, type, jsCode);
    }
    
    // 获取键信息
    std::optional<KeyInfo> getKey(const std::string& key) const {
        auto it = keys_.find(key);
        if (it != keys_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    // 获取所有键
    std::vector<std::string> getAllKeys() const {
        std::vector<std::string> result;
        for (const auto& [key, info] : keys_) {
            result.push_back(key);
        }
        return result;
    }
    
    // 设置关联的CHTL JS函数
    void setAssociatedFunction(const std::string& funcName) {
        associatedFunction_ = funcName;
    }
    
    const std::string& getAssociatedFunction() const { return associatedFunction_; }
    
private:
    std::string name_;
    std::string sourceFile_;
    std::string associatedFunction_;
    std::unordered_map<std::string, KeyInfo> keys_;
};

// 模块信息
class ModuleInfo {
public:
    ModuleInfo(const std::string& path, bool isCJJS = true)
        : path_(path), isCJJS_(isCJJS) {}
    
    const std::string& getPath() const { return path_; }
    bool isCJJS() const { return isCJJS_; }
    
    // 添加依赖
    void addDependency(const std::string& dep) {
        dependencies_.insert(dep);
    }
    
    const std::unordered_set<std::string>& getDependencies() const {
        return dependencies_;
    }
    
private:
    std::string path_;
    bool isCJJS_;
    std::unordered_set<std::string> dependencies_;
};

// 事件委托信息
class DelegationInfo {
public:
    DelegationInfo(const std::string& parentSelector)
        : parentSelector_(parentSelector) {}
    
    struct DelegationTarget {
        std::string selector;
        std::unordered_map<std::string, std::string> eventHandlers;  // event -> handler
    };
    
    void addTarget(const std::string& selector, const std::string& event, 
                   const std::string& handler) {
        targets_[selector].eventHandlers[event] = handler;
    }
    
    const std::unordered_map<std::string, DelegationTarget>& getTargets() const {
        return targets_;
    }
    
private:
    std::string parentSelector_;
    std::unordered_map<std::string, DelegationTarget> targets_;
};

// CHTL JS全局映射表
class GlobalMap {
public:
    static GlobalMap& getInstance() {
        static GlobalMap instance;
        return instance;
    }
    
    // 虚对象管理
    void registerVirtualObject(const std::string& name, const std::string& file);
    std::shared_ptr<VirtualObjectInfo> getVirtualObject(const std::string& name) const;
    
    // 模块管理
    void registerModule(const std::string& path, bool isCJJS = true);
    std::shared_ptr<ModuleInfo> getModule(const std::string& path) const;
    void addModuleDependency(const std::string& module, const std::string& dependency);
    
    // 事件委托管理
    void registerDelegation(const std::string& parentSelector);
    std::shared_ptr<DelegationInfo> getDelegation(const std::string& parentSelector) const;
    
    // 选择器自动化管理
    struct SelectorAutoContext {
        std::string currentElement;
        std::vector<std::string> autoAddedClasses;
        std::vector<std::string> autoAddedIds;
        bool styleAutoAddedClass = false;
        bool styleAutoAddedId = false;
        bool scriptAutoAddedClass = false;
        bool scriptAutoAddedId = false;
    };
    
    void pushSelectorContext(const std::string& element);
    void popSelectorContext();
    SelectorAutoContext* getCurrentSelectorContext();
    
    // 增强选择器缓存
    void cacheSelector(const std::string& selector, const std::string& jsCode);
    std::optional<std::string> getCachedSelector(const std::string& selector) const;
    
    // CHTL JS函数注册（用于iNeverAway等）
    void registerCHTLJSFunction(const std::string& name, const std::string& type);
    bool isCHTLJSFunction(const std::string& name) const;
    
    // 清空全局映射表
    void clear();
    
private:
    GlobalMap() = default;
    GlobalMap(const GlobalMap&) = delete;
    GlobalMap& operator=(const GlobalMap&) = delete;
    
    // 虚对象映射
    std::unordered_map<std::string, std::shared_ptr<VirtualObjectInfo>> virtualObjects_;
    
    // 模块映射
    std::unordered_map<std::string, std::shared_ptr<ModuleInfo>> modules_;
    
    // 事件委托映射
    std::unordered_map<std::string, std::shared_ptr<DelegationInfo>> delegations_;
    
    // 选择器上下文栈
    std::vector<SelectorAutoContext> selectorContextStack_;
    
    // 选择器缓存
    std::unordered_map<std::string, std::string> selectorCache_;
    
    // CHTL JS函数注册表
    std::unordered_set<std::string> chtljsFunctions_;
};

} // namespace CHTLJS

#endif // CHTLJS_GLOBAL_MAP_H