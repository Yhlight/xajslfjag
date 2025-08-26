#pragma once

#include "BaseNode/BaseNode.h"

namespace CHTL {
namespace Node {

/**
 * OriginNode - 原始嵌入节点
 * 处理[Origin] @Html, @Style, @JavaScript等原始嵌入
 */
class OriginNode : public BaseNode {
public:
    enum class OriginType {
        HTML,           // @Html
        STYLE,          // @Style
        JAVASCRIPT,     // @JavaScript
        CUSTOM_TYPE     // 自定义类型（如@Vue）
    };

private:
    OriginType origin_type_;
    std::string custom_type_name_;      // 自定义类型名称（非标准类型时）
    std::string origin_content_;
    std::string alias_name_;            // as语法的别名
    bool has_alias_;                    // 是否有别名
    bool is_named_embed_;              // 是否为带名原始嵌入
    
public:
    /**
     * 构造函数
     * @param type 原始嵌入类型
     * @param content 原始内容
     * @param location 源文件位置
     * @param line 行号
     * @param column 列号
     */
    OriginNode(OriginType type,
               const std::string& content,
               const std::string& location = "", 
               int line = 0, 
               int column = 0);
    
    /**
     * 自定义类型构造函数
     * @param custom_type 自定义类型名称
     * @param content 原始内容
     * @param location 源文件位置
     * @param line 行号
     * @param column 列号
     */
    OriginNode(const std::string& custom_type,
               const std::string& content,
               const std::string& location = "", 
               int line = 0, 
               int column = 0);
    
    /**
     * 获取原始嵌入类型
     * @return 原始嵌入类型
     */
    OriginType getOriginType() const;
    
    /**
     * 获取自定义类型名称
     * @return 自定义类型名称
     */
    const std::string& getCustomTypeName() const;
    
    /**
     * 设置自定义类型名称
     * @param type_name 自定义类型名称
     */
    void setCustomTypeName(const std::string& type_name);
    
    /**
     * 获取原始内容
     * @return 原始内容
     */
    const std::string& getOriginContent() const;
    
    /**
     * 设置原始内容
     * @param content 原始内容
     */
    void setOriginContent(const std::string& content);
    
    /**
     * 获取别名
     * @return 别名
     */
    const std::string& getAlias() const;
    
    /**
     * 设置别名
     * @param alias 别名
     */
    void setAlias(const std::string& alias);
    
    /**
     * 是否有别名
     * @return 是否有别名
     */
    bool hasAlias() const;
    
    /**
     * 是否为带名原始嵌入
     * @return 是否为带名嵌入
     */
    bool isNamedEmbed() const;
    
    /**
     * 设置是否为带名原始嵌入
     * @param is_named 是否为带名嵌入
     */
    void setNamedEmbed(bool is_named);
    
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
     * 生成最终代码
     * @return 生成的代码
     */
    std::string generateCode() const;
    
    /**
     * 获取完整的类型名称
     * @return 类型名称
     */
    std::string getFullTypeName() const;
    
    /**
     * 原始嵌入类型转字符串
     * @param type 原始嵌入类型
     * @return 字符串
     */
    static std::string originTypeToString(OriginType type);
    
    /**
     * 字符串转原始嵌入类型
     * @param type_str 类型字符串
     * @return 原始嵌入类型
     */
    static OriginType stringToOriginType(const std::string& type_str);
};

} // namespace Node
} // namespace CHTL