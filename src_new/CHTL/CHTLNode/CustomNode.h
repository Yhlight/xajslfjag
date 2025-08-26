#pragma once

#include "BaseNode/BaseNode.h"

namespace CHTL {
namespace Node {

/**
 * CustomNode - 自定义节点
 * 处理[Custom] @Style, @Element, @Var定义
 */
class CustomNode : public BaseNode {
public:
    enum class CustomType {
        STYLE,      // @Style
        ELEMENT,    // @Element
        VAR         // @Var
    };

private:
    CustomType custom_type_;
    std::string custom_name_;
    std::string custom_definition_;
    std::string inherit_from_;              // 继承来源
    std::vector<std::string> delete_items_; // 删除的项目
    bool is_exported_;
    
public:
    /**
     * 构造函数
     * @param type 自定义类型
     * @param name 自定义名称
     * @param location 源文件位置
     * @param line 行号
     * @param column 列号
     */
    CustomNode(CustomType type,
               const std::string& name,
               const std::string& location = "", 
               int line = 0, 
               int column = 0);
    
    /**
     * 获取自定义类型
     * @return 自定义类型
     */
    CustomType getCustomType() const;
    
    /**
     * 获取自定义名称
     * @return 自定义名称
     */
    const std::string& getCustomName() const;
    
    /**
     * 设置自定义名称
     * @param name 自定义名称
     */
    void setCustomName(const std::string& name);
    
    /**
     * 获取自定义定义
     * @return 自定义定义
     */
    const std::string& getCustomDefinition() const;
    
    /**
     * 设置自定义定义
     * @param definition 自定义定义
     */
    void setCustomDefinition(const std::string& definition);
    
    /**
     * 获取继承来源
     * @return 继承来源
     */
    const std::string& getInheritFrom() const;
    
    /**
     * 设置继承来源
     * @param inherit_from 继承来源
     */
    void setInheritFrom(const std::string& inherit_from);
    
    /**
     * 添加删除项目
     * @param item 删除项目
     */
    void addDeleteItem(const std::string& item);
    
    /**
     * 获取删除项目列表
     * @return 删除项目列表
     */
    const std::vector<std::string>& getDeleteItems() const;
    
    /**
     * 是否被导出
     * @return 是否导出
     */
    bool isExported() const;
    
    /**
     * 设置导出状态
     * @param exported 是否导出
     */
    void setExported(bool exported);
    
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
     * 生成HTML/CSS/JS代码
     * @return 生成的代码
     */
    std::string generateCode() const;
    
    /**
     * 自定义类型转字符串
     * @param type 自定义类型
     * @return 字符串
     */
    static std::string customTypeToString(CustomType type);
};

} // namespace Node
} // namespace CHTL