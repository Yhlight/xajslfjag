#ifndef CHTLJS_VIRTUAL_OBJECT_MANAGER_H
#define CHTLJS_VIRTUAL_OBJECT_MANAGER_H

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace CHTLJS {

// 虚对象定义
class VirtualObject {
public:
    VirtualObject(const std::string& name) : name_(name) {}
    virtual ~VirtualObject() = default;
    
    const std::string& getName() const { return name_; }
    
    // 添加属性
    void addProperty(const std::string& key, const std::string& value) {
        properties_[key] = value;
    }
    
    // 获取属性
    std::string getProperty(const std::string& key) const {
        auto it = properties_.find(key);
        return it != properties_.end() ? it->second : "";
    }
    
private:
    std::string name_;
    std::unordered_map<std::string, std::string> properties_;
};

// 虚对象管理器
class VirtualObjectManager {
public:
    VirtualObjectManager();
    ~VirtualObjectManager();
    
    // 注册虚对象
    void registerVirtualObject(const std::string& name, 
                             std::shared_ptr<VirtualObject> obj);
    
    // 获取虚对象
    std::shared_ptr<VirtualObject> getVirtualObject(const std::string& name);
    
    // 检查虚对象是否存在
    bool hasVirtualObject(const std::string& name) const;
    
    // 删除虚对象
    void removeVirtualObject(const std::string& name);
    
    // 获取所有虚对象名称
    std::vector<std::string> getAllVirtualObjectNames() const;
    
    // 清空所有虚对象
    void clearAll();
    
private:
    std::unordered_map<std::string, std::shared_ptr<VirtualObject>> virtualObjects_;
};

} // namespace CHTLJS

#endif // CHTLJS_VIRTUAL_OBJECT_MANAGER_H