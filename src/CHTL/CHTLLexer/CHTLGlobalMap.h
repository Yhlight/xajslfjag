#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include "CHTLToken.h"

namespace CHTL {

// CHTL关键字映射
class CHTLKeywordMap {
public:
    CHTLKeywordMap();
    
    // 检查是否为关键字
    bool isKeyword(const std::string& text) const;
    
    // 获取关键字对应的Token类型
    CHTLTokenType getKeywordType(const std::string& text) const;
    
    // 添加关键字
    void addKeyword(const std::string& keyword, CHTLTokenType type);
    
    // 移除关键字
    void removeKeyword(const std::string& keyword);
    
    // 获取所有关键字
    std::vector<std::string> getAllKeywords() const;

private:
    std::unordered_map<std::string, CHTLTokenType> keywordMap;
    void initializeDefaultKeywords();
};

// HTML标签映射
class CHTLHtmlTagMap {
public:
    CHTLHtmlTagMap();
    
    // 检查是否为有效的HTML标签
    bool isValidHtmlTag(const std::string& tagName) const;
    
    // 检查是否为自闭合标签
    bool isSelfClosingTag(const std::string& tagName) const;
    
    // 检查是否为块级元素
    bool isBlockElement(const std::string& tagName) const;
    
    // 检查是否为行内元素
    bool isInlineElement(const std::string& tagName) const;
    
    // 添加自定义标签
    void addCustomTag(const std::string& tagName, bool isSelfClosing = false, bool isBlock = false);

private:
    std::unordered_set<std::string> htmlTags;
    std::unordered_set<std::string> selfClosingTags;
    std::unordered_set<std::string> blockElements;
    std::unordered_set<std::string> inlineElements;
    
    void initializeHtmlTags();
    void initializeSelfClosingTags();
    void initializeBlockElements();
    void initializeInlineElements();
};

// CHTL模板和自定义类型映射
class CHTLTypeMap {
public:
    struct TypeInfo {
        std::string name;
        std::string namespace_;
        bool isTemplate;
        bool isCustom;
        CHTLTokenType markerType; // @Style, @Element, @Var等
    };
    
    CHTLTypeMap();
    
    // 注册模板类型
    void registerTemplate(const std::string& name, CHTLTokenType markerType, const std::string& namespace_ = "");
    
    // 注册自定义类型
    void registerCustom(const std::string& name, CHTLTokenType markerType, const std::string& namespace_ = "");
    
    // 检查类型是否存在
    bool hasType(const std::string& name, const std::string& namespace_ = "") const;
    
    // 获取类型信息
    const TypeInfo* getTypeInfo(const std::string& name, const std::string& namespace_ = "") const;
    
    // 移除类型
    void removeType(const std::string& name, const std::string& namespace_ = "");
    
    // 获取指定命名空间的所有类型
    std::vector<TypeInfo> getTypesInNamespace(const std::string& namespace_) const;

private:
    std::unordered_map<std::string, TypeInfo> typeMap;
    std::string makeKey(const std::string& name, const std::string& namespace_) const;
};

// CHTL配置映射
class CHTLConfigMap {
public:
    struct ConfigValue {
        std::string stringValue;
        bool boolValue;
        int intValue;
        std::vector<std::string> arrayValue;
        enum { STRING, BOOL, INT, ARRAY } type;
    };
    
    CHTLConfigMap();
    
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
    
    // 获取所有配置键
    std::vector<std::string> getAllKeys() const;

private:
    std::unordered_map<std::string, ConfigValue> configMap;
    void initializeDefaultConfigs();
};

// CHTL全局映射表
class CHTLGlobalMap {
public:
    CHTLGlobalMap();
    ~CHTLGlobalMap() = default;
    
    // 获取子映射表
    CHTLKeywordMap& getKeywordMap() { return keywordMap; }
    const CHTLKeywordMap& getKeywordMap() const { return keywordMap; }
    
    CHTLHtmlTagMap& getHtmlTagMap() { return htmlTagMap; }
    const CHTLHtmlTagMap& getHtmlTagMap() const { return htmlTagMap; }
    
    CHTLTypeMap& getTypeMap() { return typeMap; }
    const CHTLTypeMap& getTypeMap() const { return typeMap; }
    
    CHTLConfigMap& getConfigMap() { return configMap; }
    const CHTLConfigMap& getConfigMap() const { return configMap; }
    
    // 重置所有映射表
    void reset();
    
    // 从配置文件加载设置
    bool loadFromConfigFile(const std::string& configFilePath);
    
    // 保存配置到文件
    bool saveToConfigFile(const std::string& configFilePath) const;

private:
    CHTLKeywordMap keywordMap;
    CHTLHtmlTagMap htmlTagMap;
    CHTLTypeMap typeMap;
    CHTLConfigMap configMap;
};

} // namespace CHTL