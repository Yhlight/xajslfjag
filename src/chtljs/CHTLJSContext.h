#ifndef CHTLJS_CONTEXT_H
#define CHTLJS_CONTEXT_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../chtl/CHTLContext.h"

namespace chtl {
namespace js {

// 前向声明
class VirtualObject;
class EventDelegation;
class AnimationConfig;

// CHTL JS编译上下文
class CHTLJSContext : public CHTLContext {
public:
    CHTLJSContext();
    explicit CHTLJSContext(const CompileOptions& options);
    ~CHTLJSContext();
    
    // 虚对象管理
    void registerVirtualObject(const std::string& name, std::shared_ptr<VirtualObject> vir);
    std::shared_ptr<VirtualObject> getVirtualObject(const std::string& name) const;
    bool hasVirtualObject(const std::string& name) const;
    
    // 选择器管理
    std::string generateSelectorFunction(const std::string& selector);
    void registerAutoSelector(const std::string& selector, const std::string& elementPath);
    bool shouldAutoAddSelector(const std::string& selector) const;
    
    // 事件委托管理
    void registerEventDelegation(const std::string& parent, std::shared_ptr<EventDelegation> delegation);
    std::vector<std::shared_ptr<EventDelegation>> getEventDelegations(const std::string& parent) const;
    std::string generateDelegationCode();
    
    // 动画管理
    void registerAnimation(const std::string& name, std::shared_ptr<AnimationConfig> animation);
    std::shared_ptr<AnimationConfig> getAnimation(const std::string& name) const;
    std::string generateAnimationCode(const std::string& name);
    
    // 代码生成辅助
    std::string wrapInIIFE(const std::string& code) const;
    std::string generateDOMReady(const std::string& code) const;
    std::string generateSelectorCode(const std::string& selector, bool multiple = false) const;
    
    // CHTL JS特有的自动化规则
    bool canAutoAddClassInScript() const;
    bool canAutoAddIdInScript() const;
    std::string getPreferredSelectorType() const; // class还是id
    
    // 全局函数注册表（用于vir对象）
    void registerGlobalFunction(const std::string& name, const std::string& code);
    std::string getGlobalFunction(const std::string& name) const;
    bool hasGlobalFunction(const std::string& name) const;
    std::string generateGlobalFunctions() const;
    
    // 增强选择器缓存
    void cacheSelector(const std::string& selector, const std::string& varName);
    std::string getCachedSelector(const std::string& selector) const;
    bool hasCachedSelector(const std::string& selector) const;
    
    // 事件名称验证
    bool isValidEventName(const std::string& name) const;
    std::vector<std::string> getSupportedEvents() const;
    
    // 统计信息（CHTL JS特有）
    void incrementSelectorUsage(const std::string& selector);
    void incrementEventUsage(const std::string& event);
    std::unordered_map<std::string, size_t> getSelectorStatistics() const;
    std::unordered_map<std::string, size_t> getEventStatistics() const;
    
private:
    // 虚对象映射表
    std::unordered_map<std::string, std::shared_ptr<VirtualObject>> virtualObjects;
    
    // 事件委托表
    std::unordered_map<std::string, std::vector<std::shared_ptr<EventDelegation>>> eventDelegations;
    
    // 动画配置表
    std::unordered_map<std::string, std::shared_ptr<AnimationConfig>> animations;
    
    // 全局函数表
    std::unordered_map<std::string, std::string> globalFunctions;
    size_t globalFunctionCounter;
    
    // 选择器缓存
    std::unordered_map<std::string, std::string> selectorCache;
    size_t selectorVarCounter;
    
    // 自动选择器映射
    std::unordered_map<std::string, std::string> autoSelectors;
    
    // 支持的事件列表
    static const std::unordered_set<std::string> supportedEvents;
    
    // 统计信息
    std::unordered_map<std::string, size_t> selectorUsage;
    std::unordered_map<std::string, size_t> eventUsage;
    
    // 辅助方法
    std::string generateUniqueVarName(const std::string& prefix) const;
    std::string sanitizeSelector(const std::string& selector) const;
};

// 虚对象定义
class VirtualObject {
public:
    VirtualObject(const std::string& name) : name(name) {}
    virtual ~VirtualObject() = default;
    
    const std::string& getName() const { return name; }
    
    // 添加方法
    void addMethod(const std::string& methodName, const std::string& functionName);
    void addProperty(const std::string& propName, const std::string& value);
    
    // 获取方法和属性
    std::string getMethod(const std::string& methodName) const;
    std::string getProperty(const std::string& propName) const;
    
    // 生成代码
    std::string generateAccessCode(const std::string& member) const;
    
private:
    std::string name;
    std::unordered_map<std::string, std::string> methods;
    std::unordered_map<std::string, std::string> properties;
};

// 事件委托配置
struct EventDelegation {
    std::string parentSelector;
    std::vector<std::string> targetSelectors;
    std::unordered_map<std::string, std::string> eventHandlers; // event -> handler
    
    std::string generateCode() const;
};

// 动画配置
struct AnimationConfig {
    std::vector<std::string> targets;
    double duration = 1000;
    std::string easing = "ease-in-out";
    std::unordered_map<std::string, std::string> beginState;
    std::unordered_map<std::string, std::string> endState;
    std::vector<std::pair<double, std::unordered_map<std::string, std::string>>> keyframes;
    int loop = 1;
    std::string direction = "normal";
    double delay = 0;
    std::string callback;
    
    std::string generateCode() const;
};

} // namespace js
} // namespace chtl

#endif // CHTLJS_CONTEXT_H