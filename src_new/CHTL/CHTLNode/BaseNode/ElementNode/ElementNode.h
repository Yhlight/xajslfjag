#pragma once

#include "../BaseNode.h"

namespace CHTL {
namespace Node {

/**
 * ElementNode - HTML元素节点
 * 新目录结构下的元素节点实现
 */
class ElementNode : public BaseNode {
private:
    std::string tag_name_;
    bool is_self_closing_;
    bool is_custom_element_;
    std::string index_access_;  // [1], [0] 等索引访问
    
public:
    /**
     * 构造函数
     * @param tag_name 标签名
     * @param location 源文件位置
     * @param line 行号
     * @param column 列号
     */
    ElementNode(const std::string& tag_name, 
                const std::string& location = "", 
                int line = 0, 
                int column = 0);
    
    /**
     * 获取标签名
     * @return 标签名
     */
    const std::string& getTagName() const;
    
    /**
     * 设置标签名
     * @param tag_name 标签名
     */
    void setTagName(const std::string& tag_name);
    
    /**
     * 是否为自闭合标签
     * @return 是否自闭合
     */
    bool isSelfClosing() const;
    
    /**
     * 设置是否自闭合
     * @param self_closing 是否自闭合
     */
    void setSelfClosing(bool self_closing);
    
    /**
     * 是否为自定义元素
     * @return 是否为自定义元素
     */
    bool isCustomElement() const;
    
    /**
     * 设置是否为自定义元素
     * @param is_custom 是否为自定义元素
     */
    void setCustomElement(bool is_custom);
    
    /**
     * 获取索引访问
     * @return 索引访问字符串
     */
    const std::string& getIndexAccess() const;
    
    /**
     * 设置索引访问
     * @param index_access 索引访问字符串
     */
    void setIndexAccess(const std::string& index_access);
    
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
     * 生成HTML
     * @return HTML字符串
     */
    std::string generateHTML() const;
    
    /**
     * 获取完整的选择器
     * @return CSS选择器
     */
    std::string getSelector() const;
};

} // namespace Node
} // namespace CHTL