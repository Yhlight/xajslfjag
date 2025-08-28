#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include "CHTLJSToken.h"

namespace CHTLJS {

// CHTL JS关键字映射
class CHTLJSKeywordMap {
public:
    CHTLJSKeywordMap();
    
    // 检查是否为关键字
    bool isKeyword(const std::string& text) const;
    
    // 获取关键字对应的Token类型
    CHTLJSTokenType getKeywordType(const std::string& text) const;
    
    // 添加关键字
    void addKeyword(const std::string& keyword, CHTLJSTokenType type);
    
    // 移除关键字
    void removeKeyword(const std::string& keyword);
    
    // 获取所有关键字
    std::vector<std::string> getAllKeywords() const;
    
    // 检查是否为CHTL JS特有关键字
    bool isCHTLJSSpecificKeyword(const std::string& text) const;

private:
    std::unordered_map<std::string, CHTLJSTokenType> keywordMap;
    std::unordered_set<std::string> chtljsSpecificKeywords;
    void initializeDefaultKeywords();
    void initializeCHTLJSSpecificKeywords();
};

// CHTL JS事件映射
class CHTLJSEventMap {
public:
    CHTLJSEventMap();
    
    // 检查是否为有效的DOM事件
    bool isValidDOMEvent(const std::string& eventName) const;
    
    // 检查是否为鼠标事件
    bool isMouseEvent(const std::string& eventName) const;
    
    // 检查是否为键盘事件
    bool isKeyboardEvent(const std::string& eventName) const;
    
    // 检查是否为表单事件
    bool isFormEvent(const std::string& eventName) const;
    
    // 检查是否为触摸事件
    bool isTouchEvent(const std::string& eventName) const;
    
    // 添加自定义事件
    void addCustomEvent(const std::string& eventName);
    
    // 获取事件分类
    std::string getEventCategory(const std::string& eventName) const;

private:
    std::unordered_set<std::string> mouseEvents;
    std::unordered_set<std::string> keyboardEvents;
    std::unordered_set<std::string> formEvents;
    std::unordered_set<std::string> touchEvents;
    std::unordered_set<std::string> customEvents;
    
    void initializeMouseEvents();
    void initializeKeyboardEvents();
    void initializeFormEvents();
    void initializeTouchEvents();
};

// CHTL JS模块映射
class CHTLJSModuleMap {
public:
    struct ModuleInfo {
        std::string name;
        std::string path;
        bool isLoaded;
        std::vector<std::string> dependencies;
    };
    
    CHTLJSModuleMap();
    
    // 注册模块
    void registerModule(const std::string& name, const std::string& path);
    
    // 检查模块是否存在
    bool hasModule(const std::string& name) const;
    
    // 获取模块信息
    const ModuleInfo* getModuleInfo(const std::string& name) const;
    
    // 设置模块加载状态
    void setModuleLoaded(const std::string& name, bool loaded);
    
    // 添加模块依赖
    void addModuleDependency(const std::string& module, const std::string& dependency);
    
    // 获取模块加载顺序
    std::vector<std::string> getLoadOrder() const;
    
    // 检测循环依赖
    bool hasCircularDependency() const;

private:
    std::unordered_map<std::string, ModuleInfo> moduleMap;
    bool detectCircularDependencyRecursive(const std::string& module, 
                                          std::unordered_set<std::string>& visited,
                                          std::unordered_set<std::string>& recursionStack) const;
};

// CHTL JS虚对象映射
class CHTLJSVirtualObjectMap {
public:
    struct VirtualObject {
        std::string name;
        std::unordered_map<std::string, std::string> properties; // 属性名 -> 类型/值
        std::unordered_map<std::string, std::vector<std::string>> methods; // 方法名 -> 参数类型列表
    };
    
    CHTLJSVirtualObjectMap();
    
    // 注册虚对象
    void registerVirtualObject(const std::string& name);
    
    // 添加虚对象属性
    void addProperty(const std::string& objectName, const std::string& propertyName, const std::string& type);
    
    // 添加虚对象方法
    void addMethod(const std::string& objectName, const std::string& methodName, const std::vector<std::string>& paramTypes);
    
    // 检查虚对象是否存在
    bool hasVirtualObject(const std::string& name) const;
    
    // 获取虚对象信息
    const VirtualObject* getVirtualObject(const std::string& name) const;
    
    // 检查虚对象是否有指定属性
    bool hasProperty(const std::string& objectName, const std::string& propertyName) const;
    
    // 检查虚对象是否有指定方法
    bool hasMethod(const std::string& objectName, const std::string& methodName) const;

private:
    std::unordered_map<std::string, VirtualObject> virtualObjects;
};

// CHTL JS配置映射
class CHTLJSConfigMap {
public:
    struct ConfigValue {
        std::string stringValue;
        bool boolValue;
        int intValue;
        std::vector<std::string> arrayValue;
        enum { STRING, BOOL, INT, ARRAY } type;
    };
    
    CHTLJSConfigMap();
    
    // 设置配置值
    void setConfig(const std::string& key, const std::string& value);
    void setConfig(const std::string& key, bool value);
    void setConfig(const std::string& key, int value);
    void setConfig(const std::string& key, const std::vector<std::string>& value);
    
    // 获取配置值
    std::string getStringConfig(const std::string& key, const std::string& defaultValue = "") const;
    bool getBoolConfig(const std::string& key, bool defaultValue = false) const;
    int getIntConfig(const std::string& key, int defaultValue = 0) const;
    std::vector<std::string> getArrayConfig(const std::string& key) const;
    
    // 检查配置是否存在
    bool hasConfig(const std::string& key) const;
    
    // 移除配置
    void removeConfig(const std::string& key);

private:
    std::unordered_map<std::string, ConfigValue> configMap;
    void initializeDefaultConfigs();
};

// CHTL JS全局映射表
class CHTLJSGlobalMap {
public:
    CHTLJSGlobalMap();
    ~CHTLJSGlobalMap() = default;
    
    // 获取子映射表
    CHTLJSKeywordMap& getKeywordMap() { return keywordMap; }
    const CHTLJSKeywordMap& getKeywordMap() const { return keywordMap; }
    
    CHTLJSEventMap& getEventMap() { return eventMap; }
    const CHTLJSEventMap& getEventMap() const { return eventMap; }
    
    CHTLJSModuleMap& getModuleMap() { return moduleMap; }
    const CHTLJSModuleMap& getModuleMap() const { return moduleMap; }
    
    CHTLJSVirtualObjectMap& getVirtualObjectMap() { return virtualObjectMap; }
    const CHTLJSVirtualObjectMap& getVirtualObjectMap() const { return virtualObjectMap; }
    
    CHTLJSConfigMap& getConfigMap() { return configMap; }
    const CHTLJSConfigMap& getConfigMap() const { return configMap; }
    
    // 重置所有映射表
    void reset();
    
    // 从配置文件加载设置
    bool loadFromConfigFile(const std::string& configFilePath);
    
    // 保存配置到文件
    bool saveToConfigFile(const std::string& configFilePath) const;

private:
    CHTLJSKeywordMap keywordMap;
    CHTLJSEventMap eventMap;
    CHTLJSModuleMap moduleMap;
    CHTLJSVirtualObjectMap virtualObjectMap;
    CHTLJSConfigMap configMap;
};

} // namespace CHTLJS