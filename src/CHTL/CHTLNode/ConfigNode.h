#pragma once
#include "BaseNode.h"
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * 配置节点类型
 */
enum class ConfigNodeType {
    MAIN_CONFIG,        // 主配置节点
    NAME_CONFIG,        // Name配置块
    ORIGINTYPE_CONFIG   // OriginType配置块
};

/**
 * 配置节点
 * 表示CHTL中的[Configuration]块
 */
class ConfigNode : public BaseNode {
public:
    /**
     * 构造函数
     */
    ConfigNode(ConfigNodeType configType = ConfigNodeType::MAIN_CONFIG, 
               const std::string& configName = "");
    
    /**
     * 析构函数
     */
    virtual ~ConfigNode() = default;
    
    // ========== 配置属性 ==========
    
    /**
     * 获取配置类型
     */
    ConfigNodeType getConfigType() const;
    
    /**
     * 获取配置名称
     */
    const std::string& getConfigName() const;
    
    /**
     * 设置配置名称
     */
    void setConfigName(const std::string& name);
    
    /**
     * 检查是否为命名配置组
     */
    bool isNamedConfig() const;
    
    // ========== 配置项管理 ==========
    
    /**
     * 设置布尔配置项
     */
    void setBoolConfig(const std::string& key, bool value);
    
    /**
     * 获取布尔配置项
     */
    bool getBoolConfig(const std::string& key, bool defaultValue = false) const;
    
    /**
     * 设置整数配置项
     */
    void setIntConfig(const std::string& key, int value);
    
    /**
     * 获取整数配置项
     */
    int getIntConfig(const std::string& key, int defaultValue = 0) const;
    
    /**
     * 设置字符串配置项
     */
    void setStringConfig(const std::string& key, const std::string& value);
    
    /**
     * 获取字符串配置项
     */
    std::string getStringConfig(const std::string& key, const std::string& defaultValue = "") const;
    
    /**
     * 设置字符串数组配置项
     */
    void setStringArrayConfig(const std::string& key, const std::vector<std::string>& values);
    
    /**
     * 获取字符串数组配置项
     */
    std::vector<std::string> getStringArrayConfig(const std::string& key) const;
    
    /**
     * 检查是否有配置项
     */
    bool hasConfig(const std::string& key) const;
    
    /**
     * 移除配置项
     */
    bool removeConfig(const std::string& key);
    
    // ========== 重写基类方法 ==========
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;
    
    /**
     * 转换为字符串
     */
    std::string toString() const override;
    
    /**
     * 验证节点
     */
    bool validate(ErrorReporter* errorReporter = nullptr) const override;

protected:
    /**
     * 内部验证方法
     */
    bool internalValidate(ErrorReporter* errorReporter) const override;

private:
    ConfigNodeType m_configType;                            // 配置类型
    std::string m_configName;                               // 配置名称
    std::unordered_map<std::string, bool> m_boolConfigs;    // 布尔配置
    std::unordered_map<std::string, int> m_intConfigs;      // 整数配置
    std::unordered_map<std::string, std::string> m_stringConfigs; // 字符串配置
    std::unordered_map<std::string, std::vector<std::string>> m_arrayConfigs; // 数组配置
};

/**
 * Name配置节点
 * 专门处理关键字名称配置
 */
class NameConfigNode : public ConfigNode {
public:
    /**
     * 构造函数
     */
    NameConfigNode();
    
    /**
     * 设置关键字名称
     */
    void setKeywordName(const std::string& keyword, const std::string& name);
    
    /**
     * 获取关键字名称
     */
    std::string getKeywordName(const std::string& keyword) const;
    
    /**
     * 设置关键字名称列表（组选项）
     */
    void setKeywordNames(const std::string& keyword, const std::vector<std::string>& names);
    
    /**
     * 获取关键字名称列表
     */
    std::vector<std::string> getKeywordNames(const std::string& keyword) const;
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;
};

/**
 * OriginType配置节点
 * 专门处理自定义原始嵌入类型配置
 */
class OriginTypeConfigNode : public ConfigNode {
public:
    /**
     * 构造函数
     */
    OriginTypeConfigNode();
    
    /**
     * 注册原始嵌入类型
     */
    void registerOriginType(const std::string& typeName, const std::string& typeValue);
    
    /**
     * 获取原始嵌入类型
     */
    std::string getOriginType(const std::string& typeName) const;
    
    /**
     * 检查是否有原始嵌入类型
     */
    bool hasOriginType(const std::string& typeName) const;
    
    /**
     * 获取所有原始嵌入类型
     */
    std::vector<std::string> getAllOriginTypes() const;
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;
};

} // namespace CHTL