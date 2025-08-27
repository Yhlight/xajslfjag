#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <set>

namespace CHTLJS {

// 前向声明
class VirtualObject;
class CHTLJSFunction;
class EventDelegation;

// CHTL JS全局映射表
class GlobalMap {
public:
    GlobalMap();
    ~GlobalMap();
    
    // 虚对象管理
    void AddVirtualObject(const std::string& name, std::shared_ptr<VirtualObject> obj);
    std::shared_ptr<VirtualObject> GetVirtualObject(const std::string& name) const;
    bool HasVirtualObject(const std::string& name) const;
    void RemoveVirtualObject(const std::string& name);
    
    // CHTL JS函数管理
    void RegisterCHTLJSFunction(const std::string& name, std::shared_ptr<CHTLJSFunction> func);
    std::shared_ptr<CHTLJSFunction> GetCHTLJSFunction(const std::string& name) const;
    bool IsCHTLJSFunction(const std::string& name) const;
    
    // 增强选择器缓存
    void CacheSelector(const std::string& selector, const std::string& jsCode);
    std::string GetCachedSelector(const std::string& selector) const;
    bool HasCachedSelector(const std::string& selector) const;
    
    // 事件委托管理
    void RegisterEventDelegation(const std::string& parentSelector, 
                                std::shared_ptr<EventDelegation> delegation);
    std::shared_ptr<EventDelegation> GetEventDelegation(const std::string& parentSelector) const;
    std::vector<std::string> GetAllDelegationParents() const;
    
    // 动画管理
    std::string GenerateAnimationId();
    void RegisterAnimation(const std::string& id, const std::string& config);
    std::string GetAnimation(const std::string& id) const;
    
    // 全局函数名生成（避免命名冲突）
    std::string GenerateUniqueFunctionName(const std::string& prefix = "chtljs_func");
    std::string GenerateUniqueVariableName(const std::string& prefix = "chtljs_var");
    
    // 清空所有映射
    void Clear();
    
private:
    // 虚对象存储
    std::unordered_map<std::string, std::shared_ptr<VirtualObject>> virtualObjects;
    
    // CHTL JS函数存储
    std::unordered_map<std::string, std::shared_ptr<CHTLJSFunction>> chtljsFunctions;
    
    // 选择器缓存
    std::unordered_map<std::string, std::string> selectorCache;
    
    // 事件委托存储
    std::unordered_map<std::string, std::shared_ptr<EventDelegation>> eventDelegations;
    
    // 动画配置存储
    std::unordered_map<std::string, std::string> animations;
    
    // 计数器
    int functionCounter;
    int variableCounter;
    int animationCounter;
};

// 虚对象类
class VirtualObject {
public:
    VirtualObject(const std::string& name);
    
    void AddProperty(const std::string& key, const std::string& value, bool isFunction = false);
    std::string GetProperty(const std::string& key) const;
    bool HasProperty(const std::string& key) const;
    bool IsFunction(const std::string& key) const;
    
    std::string GetName() const { return name; }
    std::unordered_map<std::string, std::pair<std::string, bool>> GetAllProperties() const { return properties; }
    
private:
    std::string name;
    // key -> (value, isFunction)
    std::unordered_map<std::string, std::pair<std::string, bool>> properties;
};

// CHTL JS函数定义
class CHTLJSFunction {
public:
    CHTLJSFunction(const std::string& name, const std::string& pattern);
    
    void SetPattern(const std::string& pattern) { this->pattern = pattern; }
    std::string GetPattern() const { return pattern; }
    
    void AddParameter(const std::string& param, bool required = true, bool ordered = true);
    std::vector<std::string> GetParameters() const { return parameters; }
    
    std::string GenerateJS(const std::unordered_map<std::string, std::string>& args) const;
    
private:
    std::string name;
    std::string pattern;
    std::vector<std::string> parameters;
    std::set<std::string> requiredParams;
    std::set<std::string> unorderedParams;
};

// 事件委托类
class EventDelegation {
public:
    EventDelegation(const std::string& parentSelector);
    
    void AddTarget(const std::string& targetSelector, const std::string& event, 
                  const std::string& handler);
    std::string GenerateJS() const;
    
private:
    std::string parentSelector;
    // targetSelector -> event -> handler
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> targets;
};

} // namespace CHTLJS