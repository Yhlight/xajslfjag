#pragma once

#include "BaseNode/BaseNode.h"

namespace CHTL {
namespace Node {

/**
 * ConfigNode - 配置节点
 * 处理[Configuration]块的配置定义
 */
class ConfigNode : public BaseNode {
private:
    std::unordered_map<std::string, std::string> config_items_;
    std::string config_content_;
    bool is_global_;                    // 是否为全局配置
    
public:
    /**
     * 构造函数
     * @param content 配置内容
     * @param is_global 是否为全局配置
     * @param location 源文件位置
     * @param line 行号
     * @param column 列号
     */
    ConfigNode(const std::string& content,
               bool is_global = true,
               const std::string& location = "", 
               int line = 0, 
               int column = 0);
    
    /**
     * 获取配置内容
     * @return 配置内容
     */
    const std::string& getConfigContent() const;
    
    /**
     * 设置配置内容
     * @param content 配置内容
     */
    void setConfigContent(const std::string& content);
    
    /**
     * 是否为全局配置
     * @return 是否为全局配置
     */
    bool isGlobal() const;
    
    /**
     * 设置是否为全局配置
     * @param is_global 是否为全局配置
     */
    void setGlobal(bool is_global);
    
    /**
     * 设置配置项
     * @param key 配置键
     * @param value 配置值
     */
    void setConfigItem(const std::string& key, const std::string& value);
    
    /**
     * 获取配置项
     * @param key 配置键
     * @return 配置值
     */
    std::string getConfigItem(const std::string& key) const;
    
    /**
     * 检查配置项是否存在
     * @param key 配置键
     * @return 是否存在
     */
    bool hasConfigItem(const std::string& key) const;
    
    /**
     * 获取所有配置项
     * @return 配置项映射
     */
    const std::unordered_map<std::string, std::string>& getConfigItems() const;
    
    /**
     * 转换为字符串表示
     * @return 字符串表示
     */
    std::string toString() const override;
    
    /**
     * 接受访问者模式
     * @param visitor 访问者
     */
    void accept(NodeVisitor& visitor) override;
    
    /**
     * 克隆节点
     * @return 克隆的节点
     */
    std::shared_ptr<BaseNode> clone() const override;
    
    /**
     * 解析配置内容
     */
    void parseConfigContent();
    
    /**
     * 生成JavaScript配置对象
     * @return JavaScript配置对象
     */
    std::string generateJSConfig() const;
    
    /**
     * 合并配置
     * @param other 其他配置节点
     */
    void mergeConfig(const ConfigNode& other);
};

} // namespace Node
} // namespace CHTL