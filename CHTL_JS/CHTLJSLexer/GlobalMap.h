#pragma once
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <vector>
#include <functional>

namespace CHTL_JS {

// 前向声明
class VirtualObjectNode;
class ListenNode;
class DelegateNode;
class AnimateNode;

/**
 * CHTL JS全局映射管理器
 * 负责管理CHTL JS特有的虚对象、监听器、动画等
 */
class GlobalMap {
public:
    GlobalMap() = default;
    ~GlobalMap() = default;

    // 虚对象管理
    void registerVirtualObject(const std::string& name, 
                              std::shared_ptr<VirtualObjectNode> object);
    std::shared_ptr<VirtualObjectNode> getVirtualObject(const std::string& name) const;
    bool hasVirtualObject(const std::string& name) const;
    void removeVirtualObject(const std::string& name);

    // 监听器管理
    void registerListener(const std::string& selector, const std::string& event,
                         std::shared_ptr<ListenNode> listener);
    std::vector<std::shared_ptr<ListenNode>> getListeners(const std::string& selector) const;
    bool hasListener(const std::string& selector, const std::string& event) const;

    // 事件委托管理
    void registerDelegate(const std::string& parentSelector,
                         std::shared_ptr<DelegateNode> delegate);
    std::vector<std::shared_ptr<DelegateNode>> getDelegates(const std::string& parentSelector) const;
    bool hasDelegate(const std::string& parentSelector) const;

    // 动画管理
    void registerAnimation(const std::string& name,
                          std::shared_ptr<AnimateNode> animation);
    std::shared_ptr<AnimateNode> getAnimation(const std::string& name) const;
    bool hasAnimation(const std::string& name) const;

    // 增强选择器管理
    void addEnhancedSelector(const std::string& selector, const std::string& resolvedSelector);
    std::string resolveSelector(const std::string& selector) const;
    bool isSelectorEnhanced(const std::string& selector) const;

    // 全局函数注册表 (为vir虚对象生成的全局函数)
    void registerGlobalFunction(const std::string& name, const std::string& jsCode);
    std::string getGlobalFunction(const std::string& name) const;
    std::unordered_map<std::string, std::string> getAllGlobalFunctions() const;
    void clearGlobalFunctions();

    // 事件类型支持检查
    bool isSupportedEvent(const std::string& eventType) const;
    std::vector<std::string> getSupportedEvents() const;

    // DOM元素引用计数 (用于优化选择器性能)
    void incrementSelectorUsage(const std::string& selector);
    void decrementSelectorUsage(const std::string& selector);
    size_t getSelectorUsage(const std::string& selector) const;

    // 变量组引用 (CHTL JS可以使用模板变量和自定义变量组)
    void registerVariableGroup(const std::string& groupName, 
                              const std::unordered_map<std::string, std::string>& variables);
    std::string getVariable(const std::string& groupName, const std::string& varName) const;
    bool hasVariableGroup(const std::string& groupName) const;

    // 键值对无序支持 (CHTL JS函数的键值对可以无序)
    void enableUnorderedKeyValue(bool enable = true) { unorderedKeyValue_ = enable; }
    bool isUnorderedKeyValueEnabled() const { return unorderedKeyValue_; }

    // 清理方法
    void clear();
    void clearVirtualObjects();
    void clearListeners();
    void clearDelegates();
    void clearAnimations();

private:
    // 虚对象存储
    std::unordered_map<std::string, std::shared_ptr<VirtualObjectNode>> virtualObjects_;
    
    // 监听器存储 key: "selector:event"
    std::unordered_map<std::string, std::vector<std::shared_ptr<ListenNode>>> listeners_;
    
    // 事件委托存储
    std::unordered_map<std::string, std::vector<std::shared_ptr<DelegateNode>>> delegates_;
    
    // 动画存储
    std::unordered_map<std::string, std::shared_ptr<AnimateNode>> animations_;
    
    // 增强选择器映射
    std::unordered_map<std::string, std::string> enhancedSelectors_;
    
    // 全局函数注册表
    std::unordered_map<std::string, std::string> globalFunctions_;
    
    // 选择器使用计数
    std::unordered_map<std::string, size_t> selectorUsage_;
    
    // 变量组存储
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> variableGroups_;
    
    // 设置标志
    bool unorderedKeyValue_ = true;
    
    // 支持的事件类型
    static const std::unordered_set<std::string> SUPPORTED_EVENTS;
    
    // 辅助方法
    std::string makeListenerKey(const std::string& selector, const std::string& event) const;
    std::string generateFunctionName(const std::string& baseName) const;
};

} // namespace CHTL_JS