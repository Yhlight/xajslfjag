#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include "CHTLJSToken.h"

namespace CHTLJS {

// CHTL JS全局映射表 - 完全独立的CHTL JS语法管理
class CHTLJSGlobalMap {
public:
    static CHTLJSGlobalMap& Instance();
    
    // 初始化默认映射
    void Initialize();
    
    // 关键字映射
    bool IsKeyword(const std::string& keyword) const;
    CHTLJSTokenType GetKeywordType(const std::string& keyword) const;
    
    // CHTL JS函数管理
    bool IsCHTLJSFunction(const std::string& name) const;
    void RegisterCHTLJSFunction(const std::string& name);
    void RegisterVirtualObject(const std::string& name, const std::unordered_map<std::string, std::string>& properties);
    
    // 虚对象管理
    bool IsVirtualObject(const std::string& name) const;
    bool HasVirtualProperty(const std::string& objectName, const std::string& propertyName) const;
    std::string GetVirtualPropertyType(const std::string& objectName, const std::string& propertyName) const;
    
    // 事件管理 - 支持所有JS事件
    bool IsValidEvent(const std::string& eventName) const;
    const std::unordered_set<std::string>& GetAllEvents() const;
    
    // 动画缓动函数
    bool IsValidEasingFunction(const std::string& easingName) const;
    
    // 选择器验证
    bool IsValidSelector(const std::string& selector) const;
    
    // 模块加载管理
    void RegisterLoadedModule(const std::string& modulePath);
    bool IsModuleLoaded(const std::string& modulePath) const;
    
    // CJMOD接口管理
    void RegisterCJMODFunction(const std::string& signature, const std::string& implementation);
    bool IsCJMODFunction(const std::string& name) const;
    std::string GetCJMODImplementation(const std::string& signature) const;
    
    // 清理
    void Clear();
    
private:
    CHTLJSGlobalMap() = default;
    ~CHTLJSGlobalMap() = default;
    CHTLJSGlobalMap(const CHTLJSGlobalMap&) = delete;
    CHTLJSGlobalMap& operator=(const CHTLJSGlobalMap&) = delete;
    
    // 内部数据结构
    std::unordered_map<std::string, CHTLJSTokenType> keywords_;
    std::unordered_set<std::string> chtljsFunctions_;
    std::unordered_set<std::string> virtualObjects_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> virtualProperties_;
    std::unordered_set<std::string> validEvents_;
    std::unordered_set<std::string> validEasingFunctions_;
    std::unordered_set<std::string> loadedModules_;
    std::unordered_map<std::string, std::string> cjmodFunctions_;
    
    // 初始化方法
    void InitializeKeywords();
    void InitializeCHTLJSFunctions();
    void InitializeJSEvents();
    void InitializeEasingFunctions();
};

} // namespace CHTLJS